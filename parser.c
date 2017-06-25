/* Copyright (C) 2017 Asher Blum */
#include "lexer.h"
#include "parser.h"
#include "engine.h"
#include "sc_generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

//#define DEBUG(M, ...) fprintf(stderr, "%-24.24s %5d:    ", __FILE__, __LINE__); fprintf(stderr, M, __VA_ARGS__);
#define DEBUG(M, ...)
#define SET_ERR(...) snprintf(eng->err, sizeof(eng->err), __VA_ARGS__)
#define XRET_ERR(...) SET_ERR(__VA_ARGS__); eng->err_byte = t.byteno; return -1

extern sc_class_t sin_class;
extern sc_class_t add_class;
extern sc_class_t wav_class;
extern sc_class_t nmc_class;
extern sc_class_t timer_class;
extern sc_class_t quant_class;
extern sc_class_t pink_class;
extern sc_class_t dump_class;
extern sc_class_t adsr_class;
extern sc_class_t chopsin_class;
extern sc_class_t hardsync_class;
extern sc_class_t alpha_class;
extern sc_class_t swnoise_class;
extern sc_class_t mul_class;
extern sc_class_t pulse_class;

static sc_class_t *classes[] = {
    &sin_class,
    &add_class,
    &wav_class,
    &nmc_class,
    &timer_class,
    &quant_class,
    &pink_class,
    &dump_class,
    &adsr_class,
    &chopsin_class,
    &hardsync_class,
    &alpha_class,
    &swnoise_class,
    &mul_class,
    &pulse_class,
    0
};

sc_class_t *find_class(char *name, uint len) {
    sc_class_t **p;
    for(p=classes; *p; p++) {
        if(!strncmp((*p)->name, name, len)) {
            return *p; 
        }
    }
    return 0;
}

sc_param_t *find_param(sc_class_t *cl, char *name, uint len) {
    sc_param_t *p;
    for(p=cl->params; p->name; p++) {
        if(!strncmp(p->name, name, len)) {
            return p; 
        }
    }
    return 0;
}

static void set_params_to_default(void *v) {
    sc_class_t *cl;
    sc_param_t *p;
    sc_gen_t *gen;

    gen = (sc_gen_t*)v;
    cl = gen->cl;
    DEBUG("cl=%p\n", cl);
    for(p=cl->params; p->name; p++) {
        DEBUG("name='%s'  def='%s'\n", p->name, p->def);
        if(p->def[1] == 0) { // no default
            continue;
        }
        switch(p->def[0]) {
        case 'f' : // float
            *((float*)(v + p->offset)) = atof(p->def+1);
            break;
        case 'i' :
            *((int*)(v + p->offset)) = atoi(p->def+1);
            break;
        }
    }
}

// set the block's input pointer.  

static void set_inptr(void *v, uint src_blockno, sc_param_t *param) {
    sc_gen_t *gen;

    DEBUG("Setting inptr; from block %u to param '%s' = %f\n",
        src_blockno, param->name, *((float*)(v + param->offset)));
    gen = (sc_gen_t*)v;
    gen->src_blockno[gen->nins] = src_blockno;
    gen->inptr[gen->nins++] = (float*)(v + param->offset);
}

// 0=good -1=err

int parse_pgm(apb_sound_engine_t *eng, char *pgm, uint user_tag) {
    char *p;
    p = pgm;
    tok_t t, keytok, classtok;
    sc_class_t *class;
    sc_param_t *param;
    uint size;
    char otype;
    char want; // c=class ( k=key | ')', '=', v=val
    void *v;
    sc_gen_t *gen;
    apb_sound_instr_t *inst;
    uint src_blockno;

    if(eng->ninstrs >= MAX_INSTRS) {
        return -1;
    }
    inst = &(eng->instrs[eng->ninstrs]);
    eng->ninstrs ++;

    t.lineno=1;
    t.byteno=0;
    size = 0;
    want = 'c';
    eng->err_byte = 0;
    bzero(inst, sizeof(apb_sound_instr_t));
    DEBUG("Setting user_tag=%u for instr %p\n", user_tag, inst);
    inst->user_tag = user_tag;

    while(get_token(&p, &t)) {
        //printf("type=%c\n", t.tok_type);
        switch(t.tok_type) {
        case 's' : // space
            break;
        case '(' :
            if(want != '(') {
                XRET_ERR("Unexpected '('");
            }
            want = 'k';
            break;
        case ')' :
            if(want != ')' && want != 'k') {
                XRET_ERR("Unexpected ')'");
            }
            want = 'c';
            break;
        case '=' :
            if(want != '=') {
                XRET_ERR("Unexpected '='");
            }
            want = 'v';
            break;
        case '<' :
            if(want != 'v') {
                XRET_ERR("Unexpected '<'");
            }
            set_inptr(v, inst->nblocks-2, param); // refer to prev block
            want = 'k';
            break;
        case 'w' :
            if(want == 'c') { // want class
                class = find_class(t.str, t.slen);
                if(!class) {
                    XRET_ERR("Unknown class: '%.*s'", t.slen, t.str);
                }
                //printf("cmd: %.*s  class: %p\n", t.slen, t.str, class);
                //size += class->size;
                if(inst->nblocks >= MAX_BLOCKS) {
                    XRET_ERR("Too many blocks");
                }
                v = malloc(class->size);
                bzero(v, class->size);
                gen = (sc_gen_t*)v;
                //*((sc_class_t**)v) = class;
                gen->cl = class;
                gen->instr = inst;
                set_params_to_default(v);
                inst->blocks[inst->nblocks] = v;
                inst->nblocks ++;
                classtok = t;
                want = '(';
            }
            else if(want == 'k') { // want key
                param = find_param(class, t.str, t.slen);
                if(!param) {
                    XRET_ERR("Unknown param: '%.*s' in %.*s",
                        t.slen, t.str, classtok.slen, classtok.str);
                }
                keytok = t;
                want = '=';
            }
            else {
                XRET_ERR("Unexpected word: '%.*s'", t.slen, t.str);
            }
            break;
        case 'd' :
            if(want != 'v') {
                XRET_ERR("Unexpected number: '%.*s'", t.slen, t.str);
            }
            DEBUG("setting %s (%c) = %f\n", param->name, param->def[0], t.f);

            switch(param->def[0]) {
            case 'f':
                *((float*)(v + param->offset)) = t.f;
                break;
            case 'i':
                *((int*)(v + param->offset)) = t.f;
                break;
            }
            want = 'k';
            break;
        case '?' :
            if(want != 'v') {
                XRET_ERR("Unexpected '?'");
            }
            if(inst->nbindparams >= MAX_BINDADDRS) {
                XRET_ERR("Too many bind parameters");
            }
            DEBUG("setting bindparam %u to %p\n", inst->nbindparams, (float*)(v + param->offset));
            inst->bindaddrs[inst->nbindparams] = (float*)(v + param->offset);
            inst->nbindparams ++;
            want = 'k';
            break;
        case '$' :
            if(want != 'v') {
                XRET_ERR("Unexpected '?'");
            }
            src_blockno = (int)(t.f);
            DEBUG("setting param at %p to %d...\n", (float*)(v + param->offset), src_blockno);
            set_inptr(v, src_blockno, param); // refer to prev block
            want = 'k';
            break;
        default:
            XRET_ERR("Unexpected token type '%c'", t.tok_type);
        }
    }
    //printf("size=%u\n", size);
    return 0;
}


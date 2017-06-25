/* Copyright (C) 2017 Asher Blum */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "lexer.h"
#include "parser.h"
#include "engine.h"
#include "sc_generic.h"

#define DEBUG(M, ...) fprintf(stderr, "%-24.24s %5d:    ", __FILE__, __LINE__); fprintf(stderr, M, __VA_ARGS__);

void apb_sound_instr_dump(apb_sound_instr_t *inst) {
    uint i;
    sc_gen_t *gen;

    printf("Instrument: nblocks=%u  active=%u  user_tag=%u\n",
        inst->nblocks, inst->active_p, inst->user_tag);
    for(i=0; i<inst->nblocks; i++) {
        gen = (sc_gen_t*)(inst->blocks[i]);
        printf("### classname='%s'  addr=%p  inptr-=%x\n", gen->cl->name, gen, gen->inptr[0]);
        gen->cl->dump_func(inst->blocks[i]);
        printf("..\n");
    }
}

float apb_sound_instr_tic(apb_sound_instr_t *inst) {
    uint i, j;
    sc_gen_t *gen;
    float x, output[MAX_BLOCKS];

    bzero(output, sizeof(output));
    if(!(inst->active_p)) {
        return 0;
    }
    for(i=0; i<inst->nblocks; i++) {
        gen = (sc_gen_t*)(inst->blocks[i]);
        for(j=0; j<gen->nins; j++) {
            if(gen->inptr[j]) {
                //*((float*)(gen->inptr[j])) = x; // fixme - use gen->src_blockno[j]
                *((float*)(gen->inptr[j])) = output[gen->src_blockno[j]];
            }
        }
        x = gen->cl->tic_func(inst->blocks[i]);
        output[i] = x;
        //DEBUG("block %u: out=%f\n", i, x);
    }
    return x;
}

void apb_sound_instr_free(apb_sound_instr_t *inst) {
    uint i;

    for(i=0; i<inst->nblocks; i++) {
        free(inst->blocks[i]);
    }
    inst->nblocks = 0;
}

void apb_sound_instr_init(apb_sound_instr_t *inst) {
    uint i;
    sc_gen_t *gen;

    for(i=0; i<inst->nblocks; i++) {
        //DEBUG("checking block %u\n", i);
        gen = (sc_gen_t*)(inst->blocks[i]);
        if(gen->cl->init_func) {
            //DEBUG("initing block %u\n", i);
            gen->cl->init_func(inst->blocks[i]);
        }
    }
}

void apb_sound_instr_kill(apb_sound_instr_t *inst) {
    inst->active_p=0;
}


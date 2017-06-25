/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_timer_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float t; // time, seconds
    float _rt; // remaining time in seconds
} sc_type;

static sc_param_t params[] = {
    {  Y(t),   "f1"   },
    { 0 }
};

static void init(void *v) {
    sc_type *p = (sc_type*)v;
    p->_rt = p->t;
}

static float tic(void *v) {
    sc_type *p = (sc_type*)v;

    p->_rt -= (1.0/44100.0); // fixme
    if(p->_rt < 0) {
        apb_sound_instr_kill(p->gen.instr);
    }
    return 0; // I am silent
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(t);
    DF(_rt);
}

sc_class_t timer_class = { "timer", sizeof(sc_type), params, tic, dump, init };





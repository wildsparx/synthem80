/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_swnoise_t
#include "sc_common.h"

static float C0[] = { 0.99886, 0.99332, 0.96900, 0.86650, 0.55000, -0.7616, 0.0 };
static float C1[] = { 0.0555179, 0.0750759, 0.1538520, 0.3104856, 0.5329522, 0.0168980, 0.115926 };


typedef struct {
    sc_gen_t gen;
    float amp;
    float f;
    float ph;
    uint _seed;
    uint _white;
    float _x; // last sample
} sc_type;

static sc_param_t params[] = {
    {  Y(amp),   "f1"   },
    {  Y(f),   "f100"   },
    { 0 }
};

static void init(void *v) {
    sc_type *p = (sc_type*)v;

    p->_seed = 1773; 
}

// get a sample of white noise

static inline float get_white(sc_type *p) {
    p->_seed   = (p->_seed * 196314165) + 907633515;
    p->_white  = p->_seed >> 9; 
    p->_white |= 0x40000000; 
    return ((*(float*)&(p->_white))-3.0f); 
}

static float tic(void *v) {
    sc_type *s = (sc_swnoise_t*)v;

    s->ph += s->f * TWOPI / 44100; // fixme
    if(s->ph > TWOPI) {
        s->ph = 0;
        s->_x = get_white(s);
    }

    return s->_x * s->amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(amp);
    DI(_seed);
    DI(_white);
    DF(ph);
}

sc_class_t swnoise_class = { "swnoise", sizeof(sc_type), params, tic, dump, init };





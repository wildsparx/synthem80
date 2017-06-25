/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

/* based on Paul Kellet's algorithm  http://www.firstpr.com.au/dsp/pink-noise/  */

#define sc_type sc_pink_t
#include "sc_common.h"

static float C0[] = { 0.99886, 0.99332, 0.96900, 0.86650, 0.55000, -0.7616, 0.0 };
static float C1[] = { 0.0555179, 0.0750759, 0.1538520, 0.3104856, 0.5329522, 0.0168980, 0.115926 };


typedef struct {
    sc_gen_t gen;
    float amp;
    float _b[7];
    uint _seed;
    uint _white;
} sc_type;

static sc_param_t params[] = {
    {  Y(amp),   "f1"   },
    { 0 }
};

static void init(void *v) {
    int i;
    sc_type *p = (sc_type*)v;

    p->_seed = 1773; 
    for(i=0; i<7; i++) {
        p->_b[i] = 0;
    }
}

// get a sample of white noise

static inline float get_white(sc_type *p) {
    p->_seed   = (p->_seed * 196314165) + 907633515;
    p->_white  = p->_seed >> 9; 
    p->_white |= 0x40000000; 
    return ((*(float*)&(p->_white))-3.0f); 
}

static float tic(void *v) {
    sc_type *p = (sc_pink_t*)v;
    float white, pink;
    int i;

    white = get_white(p);
    //return white;
    pink = white * 0.5362 + p->_b[6];

    for(i=0; i<6; i++) {
        p->_b[i] = C0[i] * p->_b[i] + C1[i] * white;
        pink += p->_b[i];
    }

    p->_b[6] = C1[6] * white;

    return pink * p->amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(amp);
    DI(_seed);
    DI(_white);
}

sc_class_t pink_class = { "pink", sizeof(sc_type), params, tic, dump, init };





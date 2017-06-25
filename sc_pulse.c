/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"
#define sc_type sc_pulse_t
#include "sc_common.h"

#define NSLOTS 32.0

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    float dc;
} sc_type;

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f1"   },
    {  Y(ph),  "f0"   },
    {  Y(dc),  "f0.5"   },
    { 0 }
};

static float tic(void *v) {
    sc_type *s;
    s=v;
    float f;
    uint slot, u;
    
    s->ph += s->f * TWOPI / 44100; // fixme
    while(s->ph > TWOPI) {
        s->ph -= TWOPI;
    }
    if(s->ph < s->dc * TWOPI) {
        return s->amp;
    }
    return -s->amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DF(dc);
}

sc_class_t pulse_class = { "pulse", sizeof(sc_type), params, tic, dump };





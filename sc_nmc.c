/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_nmc_data.h"
#include "sc_generic.h"
#define sc_type sc_nmc_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    int   wf;
} sc_type;

#define NSLOTS 32
#define NWAVEFORMS 8

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f0.9"   },
    {  Y(ph),  "f0"   },
    {  Y(wf),  "i0"   },
    { 0 }
};


static void dump(void *v);

static float tic(void *v) {
    sc_type *s;
    uint slot;

    s=v;
    s->ph += s->f * 2.0 * 3.14 / 44100; // fixme
    while(s->ph > TWOPI) {
        s->ph -= TWOPI;
    }
    slot = s->ph * (float)NSLOTS / TWOPI;
    return s->amp * data[(s->wf % NWAVEFORMS) * NSLOTS + slot];
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DI(wf);
}

sc_class_t nmc_class = { "nmc", sizeof(sc_type), params, tic, dump };





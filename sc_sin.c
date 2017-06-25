/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#define sc_type sc_sin_t
#include "sc_generic.h"
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    float phv;
} sc_type;

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f0.9"   },
    {  Y(ph),  "f0"   },
    { 0 }
};


static void dump(void *v);

static float tic(void *v) {
    sc_type *s;
    s=v;
    s->ph += s->f * 2.0 * 3.14 / 44100; // fixme
    //dump(v);
    return sin(s->ph) * s->amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DF(phv);
}

sc_class_t sin_class = { "sin", sizeof(sc_type), params, tic, dump };





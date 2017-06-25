/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#define sc_type sc_chopsin_t
#include "sc_generic.h"
#include "sc_common.h"

#define NSLOTS 32

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    float phv;
    float f7;
    int mask;
    float g;
} sc_type;

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f0.9"   },
    {  Y(ph),  "f0"   },
    {  Y(mask),  "i0"   },
    {  Y(g),  "f-1"   },
    {  Y(f7),  "f0"   }, // unused
    { 0 }
};


static void dump(void *v);

static float tic(void *v) {
    float f;
    sc_type *s;
    uint n, slot;

    s=v;
    s->ph += s->f * 2.0 * 3.14 / 44100; // fixme
    //dump(v);
    f = sin(s->ph) * s->amp;
    slot = (uint)(s->ph  * NSLOTS / TWOPI);
    n = 1 << slot;
    if(s->mask & n) {
        f *= s->g;
    }
    return f;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DF(phv);
    DI(mask);
    DF(g);
    DF(f7);
}

sc_class_t chopsin_class = { "chopsin", sizeof(sc_type), params, tic, dump };





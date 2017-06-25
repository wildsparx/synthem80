/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_hardsync_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float f0;
    float f1;
    float amp;
    float ph0;
    float ph1;
    float af; // accel factor - very high (1000) = harder sync
    int _catchup;
} sc_type;

static sc_param_t params[] = {
    {  Y(f0),   "f100"   },
    {  Y(f1),   "f497.97"   },
    {  Y(amp),  "f1"     },
    {  Y(ph0),  "f0"     },
    {  Y(ph1),  "f0"     },
    {  Y(af),  "f5"     },
    { 0 }
};


static float oldtic(void *v) {
    sc_type *s = (sc_type*)v;
    s=v;
    float f;
    uint slot, u;

    s->ph0 += s->f0 * TWOPI / 44100; // fixme
    while(s->ph0 > TWOPI) {
        s->ph0 -= TWOPI;
        s->ph1 = 0; // hard sync
    }

    s->ph1 += s->f1 * TWOPI / 44100; // fixme
    while(s->ph1 > TWOPI) {
        s->ph1 -= TWOPI;
    }
    //f = s->amp * (s->ph1 / PI - 1.0);
    f = s->amp * (s->ph1 > PI) ? 1.0 : -1.0; // square
    f = s->amp * sin(s->ph1);
    return f;
}

static float tic(void *v) {
    sc_type *s = (sc_type*)v;
    s=v;
    float f;
    uint slot, u;

    s->ph0 += s->f0 * TWOPI / 44100; // fixme
    while(s->ph0 > TWOPI) {
        s->ph0 -= TWOPI;
        s->_catchup = 1;
    }

    if(s->_catchup) {
        f = s->af * s->f1 * TWOPI / 44100; // fixme;
        if(f > (TWOPI - s->ph1)) {
            f = TWOPI - s->ph1;
            s->_catchup = 0;
        }   
        s->ph1 += f;
    }
    else {
        s->ph1 += s->f1 * TWOPI / 44100; // fixme
    }
    while(s->ph1 > TWOPI) {
        s->ph1 -= TWOPI;
    }
    //f = s->amp * (s->ph1 / PI - 1.0);
    f = s->amp * (s->ph1 > PI) ? 1.0 : -1.0; // square
    f = s->amp * sin(s->ph1);
    return f;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f0);
    DF(f1);
    DF(amp);
    DF(ph0);
    DF(ph1);
}

sc_class_t hardsync_class = { "hardsync", sizeof(sc_type), params, tic, dump };





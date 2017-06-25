/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"
#define sc_type sc_alpha_t
#include "sc_common.h"

/* similar to nmc #2.  if sharp==0; sin.  */

#define NSLOTS 32.0

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    float sharp;
    int tslices;
} sc_type;

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f1"   },
    {  Y(ph),  "f0"   },
    {  Y(sharp),  "f0.1875"   },
    {  Y(tslices),  "i32"   },
    { 0 }
};

// 0 < f < 1
static float quantize(int slices, float f) {
    float g;
    g = floorf(f * slices) / slices;
    return g;
}

static float tic(void *v) {
    sc_type *s;
    s=v;
    float ph, f;
    int slice;
    
    s->ph += s->f * TWOPI / 44100; // fixme
    while(s->ph > TWOPI) {
        s->ph -= TWOPI;
    }

    ph = s->ph;

    if(s->tslices) { // time quantization requested
        ph = TWOPI * quantize(s->tslices, s->ph/TWOPI);
    }

    // first half:

    if(ph < PI) {
        return s->amp * sin(2.0 * ph);
    }

    // second, short sine wave:

    if(ph < (2.0 - s->sharp) * PI) {
        return s->amp * sin(2.0 * (ph - PI) / (1.0 - s->sharp));
    }

    // linear ramp portion:

    return 1.0 - 2.0 * (ph - (2.0 - s->sharp) * PI) / (s->sharp * PI);
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DF(sharp);
    DI(tslices);
}

sc_class_t alpha_class = { "alpha", sizeof(sc_type), params, tic, dump };





/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_quant_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float in;
    int steps;
    float range;
} sc_type;

static sc_param_t params[] = {
    {  Y(in),   "f1"   },
    {  Y(steps),   "i16"   },
    {  Y(range),   "f1"   },
    { 0 }
};


static float tic(void *v) {
    sc_type *p = (sc_type*)v;
    int i;
    float f;

    i = p->in / p->range * (float)(p->steps);
    f = p->range * (float)i / (float)(p->steps);
    //printf("quant: in=%f  i=%d    f=%f\n", p->in, i, f);
    return f;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(in);
    DF(range);
    DI(steps);
}

sc_class_t quant_class = { "quant", sizeof(sc_type), params, tic, dump };





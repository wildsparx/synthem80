/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_mul_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float a;
    float b;
} sc_type;

static sc_param_t params[] = {
    {  Y(a),   "f1"   },
    {  Y(b),   "f1"   },
    { 0 }
};


static float tic(void *v) {
    sc_type *p = (sc_type*)v;
    return p->a * p->b;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(a);
    DF(b);
}

sc_class_t mul_class = { "mul", sizeof(sc_type), params, tic, dump };





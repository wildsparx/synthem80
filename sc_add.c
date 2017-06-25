/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_add_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float a;
    float b;
    float c;
    float d;
} sc_type;

static sc_param_t params[] = {
    {  Y(a),   "f1"   },
    {  Y(b),   "f0"   },
    {  Y(c),   "f0"   },
    {  Y(d),   "f0"   },
    { 0 }
};


static float tic(void *v) {
    sc_type *p = (sc_add_t*)v;
    return p->a + p->b + p->c + p->d;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(a);
    DF(b);
    DF(c);
    DF(d);
}

sc_class_t add_class = { "add", sizeof(sc_type), params, tic, dump };





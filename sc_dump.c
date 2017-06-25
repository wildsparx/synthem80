/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include <string.h>
#include "sc_generic.h"

// dump the input for debugging

#define sc_type sc_dump_t
#include "sc_common.h"

typedef struct {
    sc_gen_t gen;
    float in;
    int graph;
} sc_type;

static sc_param_t params[] = {
    {  Y(in),   "f0"   },
    {  Y(graph),   "i0"   },
    { 0 }
};

static void draw_graph(float x) {
    char buf[40];
    memset(buf, ' ', sizeof(buf));
    buf[0] = '.';
    buf[sizeof(buf)-1] = '.';
    int ind;

    if(x > 1.0)
        x = 1.0;
    if(x < -1.0)
        x = -1.0;

    ind = (int)(20.0 * (x+1.0));
    if(ind >= 40) {
        ind = 39;
    }
    buf[ind] = '*';
    printf("dump: %6.3f  %.*s\n", x, sizeof(buf), buf);
}

static float tic(void *v) {
    sc_type *p = (sc_dump_t*)v;
    if(p->graph) {
        draw_graph(p->in);
    }
    else {
        printf("dump: %f\n", p->in);
    }
    return p->in;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(in);
}

sc_class_t dump_class = { "dump", sizeof(sc_type), params, tic, dump };





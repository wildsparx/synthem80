/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"

#define sc_type sc_adsr_t
#include "sc_common.h"

#define M_ATTACK    0
#define M_DECAY     1
#define M_SUSTAIN   2
#define M_RELEASE   3
#define M_DONE      4

typedef struct {
    sc_gen_t gen;
    float in;
    float at;
    float dt;
    float st;
    float sv;
    float rt;
    float _amp;
    float _t;
    float _dy;
    int _mode;
} sc_type;

static sc_param_t params[] = {
    {  Y(in),   "f1"    },
    {  Y(at),   "f0.05" },
    {  Y(dt),   "f0.1"  },
    {  Y(st),   "f0.2"  },
    {  Y(sv),   "f0.5"  },
    {  Y(rt),   "f0.3"  },
    { 0 }
};

static float calc_dy(float dv, float seconds) {
    if(seconds == 0.0) {
        return dv/44100.0; // fixme
    }
    return dv/(seconds * 44100.0);
}

static void init(void *v) {
    sc_type *p = (sc_type*)v;
    p->_amp = 0; // start at 0 volume
    p->_t = p->at; // remaining time
    p->_dy = calc_dy(1.0, p->at);
    p->_mode = M_ATTACK;
    printf("adsr: start mode=%d   _amp=%f  _t=%f  dy=%f\n", p->_mode, p->_amp, p->_t, p->_dy);
}

static float tic(void *v) {
    sc_type *p = (sc_type*)v;

    if(p->_mode == M_DONE) {
        return 0.0;
    }
    p->_t -= 1.0/44100.0; // fixme

    if(p->_t < 0.0) {
        p->_mode ++;
        switch(p->_mode) {
        case M_DECAY:
            p->_t = p->dt;
            p->_dy = calc_dy(p->sv - p->_amp, p->dt); // down to sustain
            break;
        case M_SUSTAIN:
            p->_t = p->st;
            p->_dy = 0.0; // no vol change during sustain
            break;
        case M_RELEASE:
            p->_t = p->rt;
            p->_dy = calc_dy(-p->_amp, p->rt); // down to 0
            break;
        }
        printf("adsr: new mode=%d   _amp=%f  _t=%f  dy=%f\n", p->_mode, p->_amp, p->_t, p->_dy);
    }
    p->_amp += p->_dy;
    return p->in * p->_amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(in);
    DF(at);
    DF(dt);
    DF(st);
    DF(sv);
    DF(rt);
    DI(_mode);
    DF(_amp);
}

sc_class_t adsr_class = { "adsr", sizeof(sc_type), params, tic, dump, init };





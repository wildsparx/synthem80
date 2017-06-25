/* Copyright (C) 2017 Asher Blum */
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "parser.h"
#include "sc_generic.h"
#define sc_type sc_wav_t
#include "sc_common.h"

#define NSLOTS 32.0

typedef struct {
    sc_gen_t gen;
    float f;
    float amp;
    float ph;
    int   wf;
} sc_type;

static sc_param_t params[] = {
    {  Y(f),   "f100" },
    {  Y(amp), "f1"   },
    {  Y(ph),  "f0"   },
    {  Y(wf),  "i9"   },
    { 0 }
};

static float tic(void *v) {
    sc_type *s;
    s=v;
    float f;
    uint slot, u;
    
    s->ph += s->f * TWOPI / 44100; // fixme
    while(s->ph > TWOPI) {
        s->ph -= TWOPI;
    }
    switch(s->wf) {
    case 0 : // square
        return s->ph < PI ? s->amp : -s->amp;
    case 1 : // +ramp
        return s->amp * (s->ph / PI - 1.0);
    case 2 : // -ramp
        return s->amp * (1.0 - s->ph / PI);
    case 3 : // 10% pulse
        return s->ph < PI * 0.1 ?  s->amp :
               s->ph < PI     ? 0      :
               s->ph < PI * 1.1 ? -s->amp :
                0;
    case 4 : // triangle  
        return (s->ph < PI) ? s->amp * (2.0 * (s->ph / PI) - 1.0) :
                              s->amp * (-2.0 * (s->ph / PI) + 3.0);
    case 5 : // diamond <>
        f =  s->ph < PI ? s->ph / PI : 1.0-((s->ph-PI)/PI);
        slot = (uint)(s->ph  * NSLOTS / TWOPI);
        if(slot % 2) {
            f *= -1;
        }
        return f;
    case 6 : // christmas tree
        slot = (uint)(s->ph  * NSLOTS / TWOPI);
        u = slot % 3;
        if(u == 1) {
            return 0; // center line of xmas tree
        }
        
        f = (s->ph / TWOPI)-1.0;
        if(u == 2) {
            f *= -1;
        }
        return f;
    case 7 : // ??? // somewhat successful imitation of nmc #3
        slot = (uint)(s->ph  * NSLOTS / TWOPI);
        //f = 0.2 + 0.4 * (s->ph / TWOPI);
        f = 0.3 * sin(s->ph);
        f += 0.2 * sin(1.1 * s->ph);
        switch(slot % 4) {
        case 1:
            f += 0.3;
            break;
        case 3:
            f -= 0.3;
            break;
        }
        return f;
    }
    return sin(s->ph) * s->amp;
}

static void dump(void *v) {
    sc_type *s;

    s=v;
    DF(f);
    DF(amp);
    DF(ph);
    DI(wf);
}

sc_class_t wav_class = { "wav", sizeof(sc_type), params, tic, dump };





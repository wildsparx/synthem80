/* Copyright (C) 2017 Asher Blum */
#ifndef VV_PARSER_H
#define VV_PARSER_H

#include "lexer.h"
#include "engine.h"

//typedef unsigned int uint;

typedef float (*tic_func_t)(void *v);
typedef void (*dump_func_t)(void *v);
typedef void (*init_func_t)(void *v);

typedef struct {
    char *name;
    uint offset;
    char *def; // if default is NULL, param is mand
} sc_param_t;

typedef struct {
    char *name; // sin ramp
    uint size;
    sc_param_t *params;
    tic_func_t tic_func;
    dump_func_t dump_func;
    init_func_t init_func;
} sc_class_t;

int parse_pgm(apb_sound_engine_t *eng, char *pgm, uint user_tag);
#endif // VV_PARSER_H

/* Copyright (C) 2017 Asher Blum */
#ifndef SC_GENERIC_H
#define SC_GENERIC_H

#define BLOCK_MAX_INS 4

#include "parser.h"

typedef struct {
    sc_class_t *cl;
    apb_sound_instr_t *instr; // to kill sound
    uint nins; // # of inputs
    uint src_blockno[BLOCK_MAX_INS]; // which block's output to take
    float *inptr[BLOCK_MAX_INS]; // where to write a previous input to
} sc_gen_t;

#endif // SC_GENERIC_H

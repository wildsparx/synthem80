/* Copyright (C) 2017 Asher Blum */
#ifndef ENGINE_H
#define ENGINE_H

#define MAX_INSTRS 20
#define MAX_BINDADDRS 10
#define MAX_BLOCKS 20
#include <stdio.h>

typedef struct {
    uint active_p;
    uint user_tag;
    uint nblocks;
    uint nbindparams;
    void *bindaddrs[MAX_BINDADDRS];
    void *blocks[MAX_BLOCKS];
} apb_sound_instr_t;

typedef struct {
    char err[0x100];
    uint sample_rate; // samples per second
    int err_byte;
    int dsp; // fd
    uint ninstrs;
    apb_sound_instr_t instrs[MAX_INSTRS];
    float old_sample;
    FILE *ofh;
    uint nsamp; // how many samples written
} apb_sound_engine_t;



float apb_sound_engine_tic(apb_sound_engine_t *inst);
void apb_sound_engine_dump(apb_sound_engine_t *eng);
void apb_sound_instr_dump(apb_sound_instr_t *inst);
void apb_sound_instr_init(apb_sound_instr_t *inst);
float apb_sound_instr_tic(apb_sound_instr_t *inst);
void apb_sound_engine_init(apb_sound_engine_t *eng);
void apb_sound_engine_clear_instrs(apb_sound_engine_t *eng);
void apb_sound_instr_free(apb_sound_instr_t *inst);
void apb_sound_engine_trigger(apb_sound_engine_t *eng, uint user_tag);
void apb_sound_instr_kill(apb_sound_instr_t *inst);
void apb_sound_engine_open_outfile(apb_sound_engine_t *eng, char *filename, uint max_samples);
void apb_sound_engine_close_outfile(apb_sound_engine_t *eng);
uint apb_sound_engine_get_sample_rate(apb_sound_engine_t *eng);
void apb_sound_engine_write_file(apb_sound_engine_t *eng, uint nsamples);
void apb_sound_engine_write(apb_sound_engine_t *eng);

#endif // ENGINE_H

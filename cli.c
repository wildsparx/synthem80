/* Copyright (C) 2017 Asher Blum */

/* command line interface for soundling */

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

typedef char BOOL;

/* command object, from user */

typedef struct {
    char *out_fn;
    uint ms;
    char *sound_cmd;
    BOOL verbose; 
} cmd_t;

/* execute the given command */

void imain(cmd_t *cmd) {
    char buf[0x400];
    apb_sound_engine_t eng;
    //int i, samples;
    uint i, samples, rate;

    apb_sound_engine_init(&eng);
    rate = apb_sound_engine_get_sample_rate(&eng);
    samples = cmd->ms * rate / 1000;
    if(cmd->out_fn) {
        apb_sound_engine_open_outfile(&eng, cmd->out_fn, samples);
    }

    if(parse_pgm(&eng, cmd->sound_cmd, 'a')) { /* error */
        fprintf(stderr, "%s\nat %u\n", eng.err, eng.err_byte);
    }
    else { /* ok */
        if(cmd->verbose) {
            apb_sound_engine_dump(&eng);
            printf("samples=%d\n", samples);
            printf("OK\n");
        }
        apb_sound_engine_trigger(&eng, 'a');

        if(cmd->out_fn) {
            apb_sound_engine_write_file(&eng, samples);
        }
        else { /* to dsp */
            while(eng.nsamp < samples) {
                //printf("%d < %d\n", eng.nsamp, samples);
                apb_sound_engine_write(&eng);
            }
        }
    }

    if(cmd->out_fn) {
        apb_sound_engine_close_outfile(&eng);
    }
    apb_sound_engine_clear_instrs(&eng);
}

void usage(char * prog_name) {
    fprintf(stderr, "Usage: %s [-t milliseconds] [-o outfile] 'SOUND_CMD'\n", prog_name);
    fprintf(stderr, "    -t N    Duration of output in ms; default to 1 second\n");
    fprintf(stderr, "    -o FILE Write a .sw file\n");
    fprintf(stderr, "    -v      Verbose\n");
    fprintf(stderr, "\nExample SOUND_CMD:\n");
    fprintf(stderr, "wav(f=4 amp=100 wf=9) add(a=< b=120) sin(f=< a=200) add(a=< b=150) sin(f=< amp=1)\n");
    exit(-1);
}
    
void parse_options(int argc, char **argv, cmd_t *cmd) {
    int opt;

    cmd->out_fn = 0; // default = no file
    cmd->ms = 1000;
    cmd->verbose = 0;

    while ((opt = getopt(argc, argv, "o:t:v")) != -1) {
        switch (opt) {
        case 'o':
            cmd->out_fn = optarg;
            break;
        case 't':
            cmd->ms = atoi(optarg);
            if(!cmd->ms) {
                usage(argv[0]);
            }
            break;
        case 'v':
            cmd->verbose = 1;
            break;
        default:
            usage(argv[0]);
            break;
        }
    }
    if(optind >= argc) {
        usage(argv[0]);
    }
    cmd->sound_cmd = argv[optind];
}

int main(int argc, char **argv) {
    cmd_t cmd;
    parse_options(argc, argv, &cmd);
    imain(&cmd);
}

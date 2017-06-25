/* Copyright (C) 2017 Asher Blum */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <strings.h>
#include <errno.h>
#include <sys/mman.h>
#include "engine.h"

//#define DEBUG(M, ...) fprintf(stderr, "%-24.24s %5d:    ", __FILE__, __LINE__); fprintf(stderr, M, __VA_ARGS__);
#define DEBUG(M, ...)

#define RATE 44100

void apb_sound_engine_init(apb_sound_engine_t *eng) {
    bzero(eng, sizeof(apb_sound_engine_t));
    eng->sample_rate = RATE;
    eng->dsp = 0; // output to sound card not initialized
    eng->ofh = NULL;
    eng->nsamp = 0;
}

void apb_sound_engine_init_soundcard(apb_sound_engine_t *eng) {
    int format = AFMT_S16_LE;       // 16bit little endian
    int nchannels=2;
    int samplerate=RATE;
    int setting = 0x0002000d;

    eng->dsp = open("/dev/dsp", O_WRONLY | O_NONBLOCK);

    if(!(eng->dsp)) {
        perror("open dsp");
        exit(1);
    }

    if (ioctl (eng->dsp, SNDCTL_DSP_RESET, NULL) == -1) {
        perror ("ioctl reset"); // usually means dev is busy
        exit(1);
    }

    // Set the buffer sizes and number of buffers
    if (ioctl (eng->dsp, SNDCTL_DSP_SETFRAGMENT, &setting) == -1) {
        perror ("ioctl set fragment");
        exit(1);
    }

    // Make it stereo or mono
    if (ioctl (eng->dsp, SNDCTL_DSP_STEREO, &nchannels) == -1) {
        perror ("ioctl channels");
        exit(1);
    }

    // Make it 16 bit little Endian
    if (ioctl (eng->dsp, SNDCTL_DSP_SETFMT, &format) == -1) {
        perror ("ioctl format");
        exit(1);
    }

    // Set the rate
    if (ioctl (eng->dsp, SNDCTL_DSP_SPEED, &samplerate) == -1) {
        perror ("ioctl sample rate");
        exit(1);
    }
}

uint apb_sound_engine_get_sample_rate(apb_sound_engine_t *eng) {
    return eng->sample_rate;
}

/* If we have an outfile open, write the sample to it */

static void apb_sound_engine_write_outfile(apb_sound_engine_t *eng, float fx) {
    short x;
    if(!eng->ofh) {
        return;
    }
    x = (short)(fx * 32767.0);
    fwrite(&x, sizeof(x), 1, eng->ofh);
    //printf("wrote; nsamp=%d\n", eng->nsamp);
}

/* this func is too ambiguous.  are we writing once or many times? */

void apb_sound_engine_write(apb_sound_engine_t *eng) {
    int buf[4];
    int x;
    float fx;
    int rc;

    fx = eng->old_sample;

    // open the soundcard if we haven't already
    if(!eng->dsp) {
        apb_sound_engine_init_soundcard(eng);
    }

    while(1) {
        x = (int)(fx * 32767.0);
        buf[0] = x;
        buf[1] = x;
        buf[2] = x;
        buf[3] = x;
        rc = write(eng->dsp, buf, 4);
        if(rc != 4) { // EAGAIN
            eng->old_sample = fx;
            return;
        }
        fx = apb_sound_engine_tic(eng);
        // do not write outfile here; choose dsp or file
        //apb_sound_engine_write_outfile(eng, fx);
        eng->nsamp ++;
    }
}

/* write samples to outfile, which must already be open */

void apb_sound_engine_write_file(apb_sound_engine_t *eng, uint nsamples) {
    uint i;
    float fx;

    for(i=0; i<nsamples; i++) {
        fx = apb_sound_engine_tic(eng);
        apb_sound_engine_write_outfile(eng, fx);
        eng->nsamp ++;
    }
}

void apb_sound_engine_dump(apb_sound_engine_t *eng) {
    uint i;
    for(i=0; i<eng->ninstrs; i++) {
        printf("** Instrument %u:\n", i);
        apb_sound_instr_dump(&(eng->instrs[i]));
    }
}

float apb_sound_engine_tic(apb_sound_engine_t *eng) {
    uint i;
    float x, y;

    x = 0;
    for(i=0; i<eng->ninstrs; i++) {
        y = apb_sound_instr_tic(&(eng->instrs[i]));
        //DEBUG("instr %u: %f\n", i, y);
        x += y;
    }
    return x;
}

// blow away everything; typically before shutting down

void apb_sound_engine_clear_instrs(apb_sound_engine_t *eng) {
    uint i;

    for(i=0; i<eng->ninstrs; i++) {
        apb_sound_instr_free(&(eng->instrs[i]));
    }
    eng->ninstrs = 0;
}

void apb_sound_engine_trigger(apb_sound_engine_t *eng, uint user_tag) {
    uint i;

    DEBUG("ninstrs=%u\n", eng->ninstrs);
    for(i=0; i<eng->ninstrs; i++) {
        DEBUG("Examining instr %u @ %p: active=%u  tag=%u\n",
            i, &(eng->instrs[i]), eng->instrs[i].active_p, eng->instrs[i].user_tag);
        if(!(eng->instrs[i].active_p) && eng->instrs[i].user_tag == user_tag) {
            eng->instrs[i].active_p = 1;
            apb_sound_instr_init(&(eng->instrs[i]));
            return;
        }
    }
    DEBUG("Did not find an idle instr for tag %u\n", user_tag);
    // failed
}

void apb_sound_engine_open_outfile(apb_sound_engine_t *eng, char *filename, uint max_samples) {
    if(eng->ofh) {
        fclose(eng->ofh);
    }
    eng->ofh = fopen(filename, "wb");
    if(!eng->ofh) {
        perror("Opening outfile");
        exit(-1);
    }
}
    
void apb_sound_engine_close_outfile(apb_sound_engine_t *eng) {
    if(eng->ofh) {
        fclose(eng->ofh);
    }
    eng->ofh = NULL;
}



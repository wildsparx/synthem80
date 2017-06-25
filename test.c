/* Copyright (C) 2017 Asher Blum */
#include "parser.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define POD(KEY, PGM) if(parse_pgm(&eng, PGM, KEY)) { fprintf(stderr, "%s\nat %u\n", eng.err, eng.err_byte); exit(-1); }

//char t2[] = "sin(f=27 ph=3.14 amp=1.7) add(a=2 b=300) wav(wf=3 amp=.3 f=<)";
//char t2[] = "sin(f=200 amp=1.0)";
//char t2[] = "sin(f=3 amp=100) add(a=< b=200) sin(f=< amp=1)";
//char t2[] = "wav(f=200 amp=1 wf=2)"; // tri alone
//char t2[] = "wav(f=2 amp=100 wf=2) add(a=< b=200) sin(f=< amp=1)";
//char t2[] = "sin(f=300 a=200) add(a=< b=150) sin(f=< amp=1)"; // fm
//char t2[] = "wav(f=4 amp=100 wf=9) add(a=< b=120) sin(f=< a=200) add(a=< b=150) sin(f=< amp=1)"; // fm growl
//char t2[] = "wav(f=30 amp=1 wf=6)"; 

char t2[] = "wav(f=2 amp=100 wf=2) add(a=< b=120) nmc(f=< amp=1 wf=2)";

int input_waiting() {
    int nread;
    ioctl(0, FIONREAD, &nread);
    return nread ? 1 : 0;
}

void test1() { // REP loop
    char buf[0x400];
    apb_sound_engine_t eng;
    int i;

    apb_sound_engine_init(&eng);
    printf("wav(f=100 amp=1 wf=5)\n");
    printf("wav(f=4 amp=100 wf=9) add(a=< b=120) sin(f=< a=200) add(a=< b=150) sin(f=< amp=1)\n");

    while(fgets(buf, sizeof(buf), stdin)) {
        if(parse_pgm(&eng, buf, 'a')) {
            fprintf(stderr, "%s\nat %u\n", eng.err, eng.err_byte);
        }
        else {
            apb_sound_engine_dump(&eng);
            printf("OK\n");
            apb_sound_engine_trigger(&eng, 'a');
            for(i=0; i<300000; i++) {
                apb_sound_engine_write(&eng);
            }
        }
        apb_sound_engine_clear_instrs(&eng);
    }
}

void test2() { // choose preset sounds with a, b, c, etc
    char buf[0x400];
    apb_sound_engine_t eng;
    int i;

    apb_sound_engine_init(&eng);

    POD('a', "timer(t=0.3) sin(f=300 a=200) add(a=< b=150) sin(f=< amp=0.3)")
    POD('b', "timer(t=0.8) wav(f=4 amp=100 wf=9) add(a=< b=120) sin(f=< a=200) add(a=< b=150) sin(f=< amp=0.3)")
    POD('c', "timer(t=12.0) sin(f=440 amp=0.3)")
    POD('c', "timer(t=12.0) sin(f=440 amp=0.3)")

    while(fgets(buf, sizeof(buf), stdin)) {
        apb_sound_engine_trigger(&eng, buf[0]);
        while(!input_waiting()) {
            apb_sound_engine_write(&eng);
        }
    }
    apb_sound_engine_clear_instrs(&eng);
}

int oldmain(int argc, char **argv) {
    apb_sound_engine_t eng;
    int i;

    apb_sound_engine_init(&eng);
    if(parse_pgm(&eng, t2, 'a')) {
        fprintf(stderr, "%s\nat %u\n", eng.err, eng.err_byte);
    }
    else {
        apb_sound_engine_dump(&eng);
        printf("OK\n");
        while(!input_waiting()) {
            apb_sound_engine_write(&eng);
            //printf("tic=%f\n", apb_sound_engine_tic(&eng));
        }
    }
}

void test3(int argc, char **argv) { // Run one sound from cmd line
    char buf[0x400];
    apb_sound_engine_t eng;
    int i;

    if(argc < 2) {
        fprintf(stderr, "Invoke with sound argument\n");
        fprintf(stderr, "Example: wav(f=4 amp=100 wf=9) add(a=< b=120) sin(f=< a=200) add(a=< b=150) sin(f=< amp=1)\n");
        exit(-1);
    }
    apb_sound_engine_init(&eng);
    apb_sound_engine_open_outfile(&eng, "output.sw", 44100);

    if(parse_pgm(&eng, argv[1], 'a')) {
        fprintf(stderr, "%s\nat %u\n", eng.err, eng.err_byte);
    }
    else {
        apb_sound_engine_dump(&eng);
        printf("OK\n");
        apb_sound_engine_trigger(&eng, 'a');
        for(i=0; i<300000; i++) {
            apb_sound_engine_write(&eng);
        }
    }
    apb_sound_engine_close_outfile(&eng);
    apb_sound_engine_clear_instrs(&eng);
}

void sound_sleep(apb_sound_engine_t *eng, int ms) {
    while(ms > 0) {
        //printf("ssleep: %d\n", ms);
        apb_sound_engine_write(eng);
        //printf("ssleep sleeping 1000\n");
        usleep(1000);
        ms --;
    }
}

void test4(int argc, char **argv) {
    apb_sound_engine_t eng;
    int i;

    apb_sound_engine_init(&eng);
    for(i=0; i<3; i++) {
        //if(parse_pgm(&eng, "timer(t=0.01) wav(wf=0 f=300) adsr(in=< at=0.01 dt=0.03 st=.1 sv=0.1 rt=.02)", 'a')) {
        if(parse_pgm(&eng, "timer(t=0.09) wav(wf=0 f=300) adsr(in=< at=0.01 dt=0.03 st=.1 sv=0.1 rt=.02)", 'a')) {
            fprintf(stderr, "error parsing\n");
            exit(-1);
        }
    }
    apb_sound_engine_trigger(&eng, 'a');
    for(i=0; i<10; i++) {
        sound_sleep(&eng, 50);
        apb_sound_engine_trigger(&eng, 'a');
    }
    apb_sound_engine_clear_instrs(&eng);
}

int main(int argc, char **argv) {
    test3(argc, argv);
    //test1();
}

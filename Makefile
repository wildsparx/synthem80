# Copyright (C) 2017 Asher Blum

CFLAGS=-g -O1
all: lexer.o parser.o sc_sin.o test synthem80

test: test.o lexer.o parser.o sc_sin.o sc_wav.o sc_add.o sc_nmc.o instr.o engine.o sc_timer.o sc_quant.o sc_pink.o sc_dump.o sc_adsr.o sc_chopsin.o sc_hardsync.o sc_alpha.o sc_swnoise.o sc_mul.o sc_pulse.o

	gcc $(CFLAGS) $^ -lm -o $@

synthem80: cli.o lexer.o parser.o sc_sin.o sc_wav.o sc_add.o sc_nmc.o instr.o engine.o sc_timer.o sc_quant.o sc_pink.o sc_dump.o sc_adsr.o sc_chopsin.o sc_hardsync.o sc_alpha.o sc_swnoise.o sc_mul.o sc_pulse.o
	gcc $(CFLAGS) $^ -lm -o $@

%.o: %.c
	gcc $(CFLAGS) -c $^ -o $@


clean:
	rm -f *.o test synthem80

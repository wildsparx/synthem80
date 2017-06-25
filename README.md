# synthem80

Game sound synthesizer using 1980 tech, for embedding or pre-generating sounds.

Copyright (C) 2018 Asher Blum; http://wildsparx.com

* Produces classic early-80s arcade sounds
* No UI; command driven
* Can be embedded in a game that has PCM access to sound output
* Can be run during the build phase to produce PCM files
* Uses coarse wavetables to provide fat sounds
* Uses a simple language to describe virtual circuits

## Examples

Some sounds from an 80s arcade classic.
* .sw files are 16 bit PCM, no header
* the play command is from sox
* sox can convert sw to wav

```
./synthem80 -o coin.sw 'timer(t=.192) wav(wf=4 f=4.46 ph=4.037 amp=198) add(a=< b=222) alpha(f=<)'
./synthem80 -t3000 -o normal-siren.sw  'wav(wf=4 f=2.6 a=275) add(a=< b=620) nmc(wf=6 f=<)'
./synthem80 -o eat-monster.sw 'timer(t=0.5) wav(wf=1 f=2 a=45) add(a=< b=48) nmc(wf=5 f=<)'

play -r 44100 coin.sw

```
The '<' is the output of the previous step in the circuit.

## Waveforms of wav()

Chosen by the wf parameter above.

0. Square
1. Ramp up
2. Ramp down
3. Narrow pulse
4. Triangle
5. Diamond
6. Christmas Tree
7. Special

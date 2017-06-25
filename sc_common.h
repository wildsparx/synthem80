/* Copyright (C) 2017 Asher Blum */
#ifndef SC_COMMON_H
#define SC_COMMON_H

// this file should ONLY be included in sc_*.c

#define Y(X) #X, offsetof(sc_type, X)
#define DF(X) printf("D %-20.20s    %-10.10s: %f\n", __FILE__, #X, s->X);
#define DI(X) printf("D %-20.20s    %-10.10s: %d\n", __FILE__, #X, s->X);
#define PI 3.14159
#define TWOPI (2.0 * PI)


#endif // SC_COMMON_H

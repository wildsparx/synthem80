/* Copyright (C) 2017 Asher Blum */
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

char t1[] = "sin(f=1 ph=3.14 amp=200) add(a1=< a2=300) wav(wf=3 amp=.3 f=<)";

#include "lexer.h"

int is_special_punc(char c) {
    if(index("()<=", c)) {
        return 1;
    }
    return 0;
}

char char2class(char c) {
    if(is_special_punc(c)) {
        return c;
    }
    if(c == '$') {
        return c;
    }
    if(isalpha(c) || c == '_') {
        return 'w';
    }
    if(isdigit(c) || c == '-' || c == '.') {
        return 'd';
    }
    if(isspace(c)) {
        return 's';
    }
    return '?';
}

static float my_atof(char *s, uint len) {
    char buf[0x100];
    double d;

    if(len < 1) {
        return 0;
    }

    if(len >= sizeof(buf)) {
        return -1.0;
    }
    strncpy(buf, s, len);
    buf[len]=0;
    d = atof(buf);
    //printf("my_atof: '%s' -> %f\n", buf, d);
    return (float)d;
}

int get_token(char **p, tok_t *tok) {
    int len = 0; // accumulated sequence
    char ocl = 0;
    char cl;

    while(**p) {
        cl = char2class(**p);
        tok->byteno ++;
        if(**p == '\n') {
            tok->lineno ++;
        }

        /* a digit appended to alphas is treated as alpha */

        if(cl == 'd' && (ocl == 'w' || ocl == '$')) {
            cl = ocl;
        }
        if(len && (ocl != cl || is_special_punc(cl))) {
            tok->tok_type = ocl;
            tok->slen = len;
            tok->str = (*p)-len;
            if(ocl == 'd') {
                tok->f = my_atof(tok->str, tok->slen);
                //printf("parsed '%.*s' to %f\n", tok->slen, tok->str, tok->f);
            }
            else if(ocl == '$') {
                tok->f = my_atof(tok->str+1, tok->slen-1);
            }
            //printf("tok: %c  '%.*s'\n", ocl, len, (*p)-len);
            return 1;
            len = 0;
        }
        (*p)++;
        len ++; // accumulate
        ocl = cl;
    }
    return 0;
}
        
        

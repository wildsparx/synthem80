/* Copyright (C) 2017 Asher Blum */
#ifndef VV_LEXER_H
#define VV_LEXER_H

typedef unsigned int uint;

typedef struct {
    uint lineno;
    uint byteno;
    uint slen;
    char *str;
    float f;
    char tok_type;
} tok_t;
int get_token(char **p, tok_t *tok);

#endif // VV_LEXER_H

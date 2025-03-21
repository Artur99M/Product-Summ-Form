#ifndef LEX_ANALIZER_H
#define LEX_ANALIZER_H
#include <stdio.h>
enum LEX_ANALISYS_ERROR {
     NO_ERROR
    ,UNDEFINED_LEXEM
    ,TEXT_NULLPTR
    ,MALLOC_ERROR
};

enum token_type {
     ERROR
    ,OP
    ,NUM
    ,VAR
};
union token_val {
    char var_op;
    int  num;
};

typedef struct token {
    enum token_type type;
    union token_val val;
} token;

typedef struct token_stream {
    token* arr;
    size_t size;
} token_stream;

enum LEX_ANALISYS_ERROR lex_analizer(char* text, token_stream* stream);
void                    dtor_lex    (token_stream* stream);
#endif

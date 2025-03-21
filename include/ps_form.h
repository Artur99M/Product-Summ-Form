#ifndef PS_FORM_H
#define PS_FORM_H
#include <stdbool.h>
#include "lex_analizer.h"

enum PS_FORM_ERROR {
     PS_FORM_NO_ERROR
    ,PS_FORM_STREAM_NULLPTR
    ,PS_FORM_FORM_NULLPTR
    ,PS_FORM_MALLOC_ERROR
    ,PS_FORM_UNDEFINED_LEXEM
    ,PS_FORM_UNEXPECTED_LEXEM
    ,PS_FORM_MONOME_NULLPTR
    ,PS_FORM_CANT_DIV
    ,PS_FORM_MUL_ERROR
};

typedef struct node_variable {
    struct node_variable* next;
    unsigned              degree;
    char                  var;
} node_variable;
typedef struct node_monome {
    struct node_monome* next;
    node_variable*      vars; // vars == NULL => it's a number
    int                 mult;
} node_monome;

typedef struct ps_form {
    node_monome* polynomial;
    node_monome* tail;
} ps_form;

enum PS_FORM_ERROR to_ps_form   (const token_stream* stream, ps_form* form);
void               dtor_ps_form (ps_form* form);
void               print_ps_form(FILE* file, const ps_form* form);
void               print_monome (FILE* file, const node_monome* term);
void               dtor_monome  (node_monome* monome);
void               dtor_variable(node_variable* var);
#endif

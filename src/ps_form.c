#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "def.h"
#include "lex_analizer.h"
#include "ps_form.h"

#define NUMBER_POSSIBLE_VARIABLES 26

void dtor_variable(node_variable* var) {
    if (var == NULL)
        return;
    for (node_variable *prev = NULL, *next = var->next; var != NULL; var = next) {
        var->degree = 0;
        var->var    = '\0';
        next        = var->next;
        prev        = var;
        free(prev);
    }
}
void dtor_monome(node_monome* monome) {
    if (monome == NULL)
        return;

    for (node_monome *prev = NULL, *next = monome->next; monome != NULL; monome = next) {
        dtor_variable(monome->vars);
        monome->mult = 0;
        next         = monome->next;
        prev         = monome;
        free(prev);
    }
}
void dtor_ps_form(ps_form* form) {
    dtor_monome(form->polynomial);
    form->polynomial = NULL;
    form->tail       = NULL;
}

static enum PS_FORM_ERROR read_monome(const token_stream* stream, node_monome* current, size_t* piterator_stream) {
    if (stream == NULL || piterator_stream == NULL)
        return PS_FORM_STREAM_NULLPTR;
    if (current == NULL)
        return PS_FORM_FORM_NULLPTR;

    int mult = 1;
    if (stream->arr[*piterator_stream].type == OP) {

        if (stream->arr[*piterator_stream].val.var_op == '-')
            mult = -1;

        else if (stream->arr[*piterator_stream].val.var_op != '+')
            return PS_FORM_UNEXPECTED_LEXEM;

        ++(*piterator_stream);
    }

    //here we sort variables alphabetically because it is important for division
    node_variable* in_list[NUMBER_POSSIBLE_VARIABLES] = {NULL};

    while (*piterator_stream < stream->size                    &&
           (stream->arr[*piterator_stream].type != OP          ||
           (stream->arr[*piterator_stream].val.var_op != '+'   &&
           stream->arr[*piterator_stream].val.var_op != '-' ))) {

        if (stream->arr[*piterator_stream].type == NUM) {
            mult *= stream->arr[*piterator_stream].val.num;

        } else if (stream->arr[*piterator_stream].type == VAR) {

            char val = tolower(stream->arr[*piterator_stream].val.var_op);

            if (in_list[val - 'a'] != NULL) {
                ++(in_list[val - 'a']->degree);

            } else {
                in_list[val - 'a'] = calloc(1, sizeof(node_variable));
                if (in_list[val - 'a'] == NULL) {
                    for (size_t i = 0; i < NUMBER_POSSIBLE_VARIABLES; ++i)
                        dtor_variable(in_list[i]);
                    return PS_FORM_MALLOC_ERROR;
                }

                in_list[val - 'a']->var    = val;
                in_list[val - 'a']->degree = 1;
            }

        } else if (stream->arr[*piterator_stream].type != OP ||
                   stream->arr[*piterator_stream].val.var_op != '*') {

            for (size_t i = 0; i < NUMBER_POSSIBLE_VARIABLES; ++i)
                dtor_variable(in_list[i]);
            return PS_FORM_UNDEFINED_LEXEM;
        }

        ++(*piterator_stream);
    }

    node_monome* monome = calloc(1, sizeof(node_monome));
    if (monome == NULL) {
        for (size_t i = 0; i < NUMBER_POSSIBLE_VARIABLES; ++i)
            dtor_variable(in_list[i]);
        return PS_FORM_MALLOC_ERROR;
    }

    node_variable dummy         = {NULL, 0, '\0'};
    node_variable* current_tail = &dummy;
    for (int i = 0; i < NUMBER_POSSIBLE_VARIABLES; ++i) {
        if (in_list[i] != NULL) {
            current_tail->next = in_list[i];
            current_tail       = current_tail->next;
        }
    }
    monome->vars  = dummy.next;
    monome->mult  = mult;
    current->next = monome;
    return PS_FORM_NO_ERROR;
}

enum PS_FORM_ERROR to_ps_form(const token_stream* stream, ps_form* form) {
    if (stream == NULL)
        return PS_FORM_STREAM_NULLPTR;
    if (form == NULL)
        return PS_FORM_FORM_NULLPTR;

    node_monome dummy      = {NULL, NULL, 0};
    node_monome* current   = &dummy;
    enum PS_FORM_ERROR err = PS_FORM_NO_ERROR;

    for (size_t i = 0; i < stream->size; current = current->next) {
        err = read_monome(stream, current, &i);
        if (err != PS_FORM_NO_ERROR) {
            dtor_monome(dummy.next);
            return err;
        }
    }

    form->polynomial = dummy.next;
    form->tail       = current;
    return PS_FORM_NO_ERROR;
}

void print_ps_form(FILE* file, const ps_form* form) {
    if (form == NULL || file == NULL)
        return;

    if (form->polynomial == NULL) {
        putc('0', file);
        return;
    }
    print_monome(file, form->polynomial);

    for (node_monome* iter_monome = form->polynomial->next; iter_monome != NULL; iter_monome = iter_monome->next) {

        char sign = (iter_monome->mult > 0 ? '+' : '-');
        fprintf(file, " %c ", sign);

        if (iter_monome->vars == NULL) {
            fprintf(file, "%d", iter_monome->mult);
            continue;
        }

        if (iter_monome->mult > 0 && iter_monome->mult != 1) {
            fprintf(file, "%d", iter_monome->mult);
            if (iter_monome->vars != NULL)
            putc('*', file);

        } else if (iter_monome->mult <= 0 && iter_monome->mult != -1) {
            fprintf(file, "%d", -iter_monome->mult);
            if (iter_monome->vars != NULL)
                putc('*', file);
        }

        fprintf(file, "%c", iter_monome->vars->var);
        for (unsigned k = 1; k < iter_monome->vars->degree; ++k)
            fprintf(file, "*%c", iter_monome->vars->var);

        for (node_variable* iter = iter_monome->vars->next; iter != NULL; iter = iter->next) {

            for (unsigned k = 0; k < iter->degree; ++k)
                fprintf(file, "*%c", iter->var);
        }
    }
}

void print_monome(FILE* file, const node_monome* term) {
    if (file == NULL || term == NULL)
        return;

    if (term->vars != NULL) {

        if (term->mult != 1) {
            fprintf(file, "%d", term->mult);
            putc('*', file);
        }

        fprintf(file, "%c", term->vars->var);
        for (unsigned k = 1; k < term->vars->degree; ++k)
            fprintf(file, "*%c", term->vars->var);

        for (node_variable* iter = term->vars->next; iter != NULL; iter = iter->next) {

            for (unsigned k = 0; k < iter->degree; ++k)
                fprintf(file, "*%c", iter->var);
        }

    } else
        fprintf(file, "%d", term->mult);
}

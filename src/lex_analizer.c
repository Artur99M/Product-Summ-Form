#include "lex_analizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "def.h"

static void skip_space (char** s)
{
    while (isspace(**s) || **s == '\n') {
        (*s)++;
    }
}

bool isoperator(char ch) {
    if (ch == '*' || ch == '+' || ch == '-')
        return true;
    return false;
}

enum LEX_ANALISYS_ERROR lex_analizer(char* text, token_stream* stream) {
    if (text == NULL) {
        return TEXT_NULLPTR;
    }
    size_t string_size     = strlen(text);
           stream->arr     = calloc(string_size, sizeof(token));
           stream->size    = 0;
    token* iterator_stream = stream->arr;
    if (stream->arr == NULL) {
        return MALLOC_ERROR;
    }

    for (char* iterator = text; *iterator != '\0'; ++iterator) {
        skip_space(&iterator);

        //variables
        if (isalpha(*iterator)) {
            iterator_stream->type       = VAR;
            iterator_stream->val.var_op = *iterator;
            ++iterator_stream;

        //operators
        } else if (isoperator(*iterator) && iterator_stream != stream->arr &&
            (iterator_stream - 1)->type != OP) {

            iterator_stream->type       = OP;
            iterator_stream->val.var_op = *iterator;
            ++iterator_stream;

        //numbers
        } else if ((*iterator == '+' || *iterator == '-') && isdigit(*(iterator + 1))) {
            char operator = *iterator;
            ++iterator;
            int val = 0;
            while (isdigit(*iterator)) {
                val *= 10;
                val += (*iterator - '0');
                ++iterator;
            }
            --iterator; //because while read one extra elem
            val = (operator == '+' ? val : -val);
            iterator_stream->type     = NUM;
            iterator_stream->val.num  = val;
            ++iterator_stream;

        } else if (isdigit(*iterator)) {
            int val = 0;
            while (isdigit(*iterator)) {
                val *= 10;
                val += (*iterator - '0');
                ++iterator;
            }
            --iterator;
            iterator_stream->type     = NUM;
            iterator_stream->val.num  = val;
            ++iterator_stream;

        //errors
        } else if (*iterator != '\0') { //additional check for skip_space
            free(stream->arr);
            stream->arr = NULL;
            return UNDEFINED_LEXEM;
        }
    }

    stream->size = iterator_stream - stream->arr;
    stream->arr  = realloc(stream->arr, stream->size * sizeof(token));
    return (stream->arr == NULL ? MALLOC_ERROR : NO_ERROR);
}


void dtor_lex(token_stream* stream) {
    if (stream != NULL) {
        free(stream->arr);
        stream->size = 0;
    }
}

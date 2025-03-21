#include <stdio.h>
#include <stdlib.h>
#include"lex_analizer.h"
#include "def.h"
#include "ps_form.h"
#include "ps_form_operators.h"

#define MAX_STR_STREAM_SIZE 10000

int main(void) {
    char operator;
    scanf("%c\n", &operator);

    static char input_string1[MAX_STR_STREAM_SIZE] = "";
    fgets(input_string1, MAX_STR_STREAM_SIZE, stdin);
    token_stream stream1 = {NULL, 0};
    enum LEX_ANALISYS_ERROR err1 = lex_analizer(input_string1, &stream1);

    static char input_string2[MAX_STR_STREAM_SIZE] = "";
    fgets(input_string2, MAX_STR_STREAM_SIZE, stdin);
    token_stream stream2 = {NULL, 0};
    enum LEX_ANALISYS_ERROR err2 = lex_analizer(input_string2, &stream2);

    if (err2 != NO_ERROR || err1 != NO_ERROR) {
        fprintf(stderr, "LEX_STREAM ERROR WITH CODES %d %d\n", err1, err2);
        dtor_lex(&stream1);
        dtor_lex(&stream2);
        exit(-1);
    }

    ps_form form1;
    enum PS_FORM_ERROR err_ps1 = to_ps_form(&stream1, &form1);
    ps_form form2;
    enum PS_FORM_ERROR err_ps2 = to_ps_form(&stream2, &form2);
    dtor_lex(&stream1);
    dtor_lex(&stream2);

    if (err_ps1 != PS_FORM_NO_ERROR || err_ps2 != PS_FORM_NO_ERROR) {
        fprintf(stderr, "PS_FORM ERROR WITH CODES %d %d\n", err_ps1, err_ps2);
        dtor_ps_form(&form1);
        dtor_ps_form(&form2);
        exit(-1);
    }

    enum PS_FORM_ERROR err_ps3 = PS_FORM_NO_ERROR;
    switch(operator) {
        case '+': {
            ps_form res = {NULL, NULL};
            err_ps3 = ps_form_sum(&res, &form1, &form2);
            if (err_ps3 != PS_FORM_NO_ERROR)
                fprintf(stderr, "PS_FORM ERROR WITH CODE %d\n", err_ps3);
            else
                print_ps_form(stdout, &res);
            dtor_ps_form(&res);
            break;
        }
        case '-': {
            ps_form res = {NULL, NULL};
            err_ps3 = ps_form_sub(&res, &form1, &form2);
            if (err_ps3 != PS_FORM_NO_ERROR)
                fprintf(stderr, "PS_FORM ERROR WITH CODE %d\n", err_ps3);
            else
                print_ps_form(stdout, &res);
            dtor_ps_form(&res);
            break;
        }

        case '*': {
            ps_form res = {NULL, NULL};
            err_ps3 = ps_form_mul(&res, &form1, &form2);
            if (err_ps3 != PS_FORM_NO_ERROR)
                fprintf(stderr, "PS_FORM ERROR WITH CODE %d\n", err_ps3);
            else
                print_ps_form(stdout, &res);
            dtor_ps_form(&res);
            break;
        }

        case '/': {
            ps_form res = {NULL, NULL};
            err_ps3 = ps_form_div(&res, &form1, &form2);
            if (err_ps3 == PS_FORM_CANT_DIV)
                printf("error");
            else if (err_ps3 != PS_FORM_NO_ERROR)
                fprintf(stderr, "PS_FORM ERROR WITH CODE %d\n", err_ps3);
            else {
                print_ps_form(stdout, &res);
                dtor_ps_form(&res);
            }
            break;
        }

        case '=': {
            bool res;
            err_ps3 = ps_form_equal(&res, &form1, &form2);
            if (err_ps3 != PS_FORM_NO_ERROR)
                fprintf(stderr, "PS_FORM ERROR WITH CODE %d\n", err_ps3);
            else
                printf("%s", res ? "equal" : "not equal");
            break;
        }
        default:
            printf("error");

    }
    dtor_ps_form(&form1);
    dtor_ps_form(&form2);
}


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ps_form_operators.h"
#include "def.h"

static void clean_zero_elements(ps_form* form) {
    if (form == NULL)
        return;

    node_monome    dummy = {form->polynomial, NULL, 0};;
    node_monome* current = form->polynomial;
    node_monome* prev    = &dummy;

    while (current != NULL) {
        if (current->mult == 0) {
            prev->next = current->next;
            current->next = NULL;
            dtor_monome(current);
            current = prev;
        } else {
            prev = current;
        }
        current = current->next;
    }

    form->polynomial = dummy.next;
    form->tail       = prev;
}
static enum PS_FORM_ERROR copy_vars(node_variable** res, const node_variable* copied) {
    if (res == NULL || copied == NULL)
        return PS_FORM_MONOME_NULLPTR;

    node_variable  dummy   = {NULL, 0, '\0'};
    node_variable* current = &dummy;

    for (; copied != NULL; copied = copied->next) {

        node_variable* prev    = current;
                       current = calloc(1, sizeof(node_variable));

        if (current == NULL) {
            dtor_variable(dummy.next);
            return PS_FORM_MALLOC_ERROR;
        }

        prev->next      = current;
        current->degree = copied->degree;
        current->var    = copied->var;
    }

    *res = dummy.next;
    return PS_FORM_NO_ERROR;
}

static enum PS_FORM_ERROR copy_monome(node_monome** res, const node_monome* copied) {
    if (res == NULL || copied == NULL)
        return PS_FORM_MONOME_NULLPTR;

    node_monome  dummy   = {NULL, NULL, 0};
    node_monome* current = &dummy;

    for (; copied != NULL; copied = copied->next) {

        node_monome* prev    = current;
                     current = calloc(1, sizeof(node_monome));

        if (current == NULL) {
            dtor_monome(dummy.next);
            return PS_FORM_MALLOC_ERROR;
        }

        current->mult = copied->mult;
        prev->next    = current;

        if (copied->vars != NULL) {

            enum PS_FORM_ERROR err = copy_vars(&(current->vars), copied->vars);

            if (err != PS_FORM_NO_ERROR) {
                dtor_monome(dummy.next);
                return err;
            }
        }
    }

    *res = dummy.next;
    return PS_FORM_NO_ERROR;
}

//it used when we don't want to copy one monome without childs
static enum PS_FORM_ERROR copy_one_monome(node_monome** tail, const node_monome* copied) {
    if (tail == NULL || copied == NULL)
        return PS_FORM_MONOME_NULLPTR;

    node_monome* new_monome = calloc(1, sizeof(node_monome));

    if (new_monome == NULL)
        return PS_FORM_MALLOC_ERROR;

    new_monome->mult = copied->mult;
    new_monome->next = NULL;
    new_monome->vars = NULL;

    if (copied->vars != NULL) {

        enum PS_FORM_ERROR err = copy_vars(&(new_monome->vars), copied->vars);
        if (err != PS_FORM_NO_ERROR) {
            dtor_monome(new_monome);
            return err;
        }
    }

    (*tail)->next = new_monome;

    return PS_FORM_NO_ERROR;
}

static enum PS_FORM_ERROR copy_ps_form(ps_form* res, const ps_form* copied) {
    if (res == NULL || copied == NULL)
        return PS_FORM_FORM_NULLPTR;

    enum PS_FORM_ERROR err = copy_monome(&(res->polynomial), copied->polynomial);
    if (err == PS_FORM_NO_ERROR)
        for (res->tail = res->polynomial; res->tail->next != NULL; res->tail = res->tail->next);

    return err;
}

static bool is_equal_vars(const node_monome* term1, const node_monome* term2) {
    if (term1->vars == NULL || term2->vars == NULL)
        return term1->vars == term2->vars;

    bool ans = true;
    node_variable *var1 = term1->vars, *var2 = term2->vars;

    for (; var1 != NULL && var2 != NULL; var1 = var1->next, var2 = var2->next)
        if (var1->var != var2->var || var1->degree != var2->degree) {
            ans = false;
            break;
        }

    return ans && var1 == NULL && var2 == NULL;
}

static bool is_equal_monomes(const node_monome* term1, const node_monome* term2) {
    if (term1 == NULL || term2 == NULL)
        return false;

    return is_equal_vars(term1, term2) && term1->mult == term2->mult;
}


enum PS_FORM_ERROR ps_form_equal(bool* res, const ps_form* term1, const ps_form* term2) {
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    for (node_monome *mon1 = term1->polynomial; mon1 != NULL; mon1 = mon1->next) {
        *res = false;
        for (node_monome *mon2 = term2->polynomial; mon2 != NULL; mon2 = mon2->next)
            if (is_equal_monomes(mon1, mon2)) {
                *res = true;
                break;
            }

        if (!*res) break;
    }

    return PS_FORM_NO_ERROR;
}

enum PS_FORM_ERROR ps_form_add(ps_form* term1, const ps_form* term2) {
    if (term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    node_monome* start_tail = term1->tail;
    for (node_monome* current2 = term2->polynomial; current2 != NULL; current2 = current2->next) {

        bool has_break = false;
        for (node_monome* current1 = term1->polynomial; current1 != NULL; current1 = current1->next) {

            if (is_equal_vars(current2, current1)) {
                current1->mult += current2->mult;
                has_break = true;
                break;
            }
        }

        if (!has_break) {
            enum PS_FORM_ERROR err = copy_one_monome(&(term1->tail), current2);
            if (err != PS_FORM_NO_ERROR) {
                if (start_tail != NULL)
                    dtor_monome(start_tail->next);
                term1->tail = start_tail;
                return err;
            }
            term1->tail = term1->tail->next;
        }
    }

    clean_zero_elements(term1);
    return PS_FORM_NO_ERROR;
}

enum PS_FORM_ERROR ps_form_sum(ps_form* res, const ps_form* term1, const ps_form* term2){
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    enum PS_FORM_ERROR err = copy_ps_form(res, term1);
    if (err != PS_FORM_NO_ERROR)
        return err;

    err = ps_form_add(res, term2);

    return err;
}

enum PS_FORM_ERROR ps_form_ded(ps_form* term1, const ps_form* term2) {
    if (term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    node_monome* start_tail = term1->tail;
    for (node_monome* current2 = term2->polynomial; current2 != NULL; current2 = current2->next) {

        bool has_break = false;
        for (node_monome* current1 = term1->polynomial; current1 != NULL; current1 = current1->next) {

            if (is_equal_vars(current2, current1)) {
                current1->mult -= current2->mult;
                has_break = true;
                break;
            }
        }

        if (!has_break) {
            enum PS_FORM_ERROR err = copy_one_monome(&(term1->tail), current2);
            if (err != PS_FORM_NO_ERROR) {
                if (start_tail != NULL)
                    dtor_monome(start_tail->next);
                term1->tail = start_tail;
                return err;
            }
            term1->tail = term1->tail->next;
            term1->tail->mult = -term1->tail->mult;
        }
    }

    clean_zero_elements(term1);
    return PS_FORM_NO_ERROR;
}

enum PS_FORM_ERROR ps_form_sub(ps_form* res, const ps_form* term1, const ps_form* term2){
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    enum PS_FORM_ERROR err = copy_ps_form(res, term1);
    if (err != PS_FORM_NO_ERROR)
        return err;

    err = ps_form_ded(res, term2);

    return err;
}

static enum PS_FORM_ERROR monome_mul(node_monome** res, const node_monome* term1, const node_monome* term2) {
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    node_monome        dummym    = {NULL, NULL, 0};
    node_monome*       current   = &dummym;

    enum PS_FORM_ERROR err       = copy_one_monome(&current, term1);
    if (err != PS_FORM_NO_ERROR) {
        dtor_monome(current);
        return err;
    }
    *res = current->next;

    node_variable dummy = {(*res)->vars, 0, 0},
                  *var1 = &dummy,
                  *var2 = term2->vars;

    while (var1->next != NULL && var2 != NULL) {

        if (var1->var == var2->var) {
            var1->degree += var2->degree;
            var2 = var2->next;

        } else if (var1->var < var2->var && var1->next->var > var2->var) {

            node_variable* next = var1->next;
            err = copy_vars(&(var1->next), var2);

            if (err != PS_FORM_NO_ERROR) {
                dtor_variable(dummy.next);
                (*res)->vars = NULL;
                return err;
            }

            var1->next->next = next;
            var2 = var2->next;
        }

        var1 = var1->next;
    }

    if (var2 != NULL) {

        err = copy_vars(&(var1->next), var2);

        if (err != PS_FORM_NO_ERROR) {

            dtor_variable(dummy.next);
            (*res)->vars = NULL;
            return err;
        }
    }

    (*res)->vars = dummy.next;
    (*res)->mult *= term2->mult;

    return PS_FORM_NO_ERROR;

}


enum PS_FORM_ERROR ps_form_mul(ps_form* res, const ps_form* term1, const ps_form* term2) {
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    node_monome  dummy   = {res->polynomial, NULL, 0};
    node_monome* current = &dummy;

    for (node_monome* cur1 = term1->polynomial; cur1 != NULL; cur1 = cur1->next)
        for (node_monome* cur2 = term2->polynomial; cur2 != NULL; cur2 = cur2->next) {

            enum PS_FORM_ERROR err = monome_mul(&(current->next), cur1, cur2);
            if (err != PS_FORM_NO_ERROR) {
                dtor_ps_form(res);
                return err;
            }

            res->tail = current = current->next;
        }

    res->polynomial = dummy.next;

    for (node_monome* cur = res->polynomial; cur->next != NULL; cur = cur->next)
        for (node_monome *cur2 = cur->next, *prev = cur; cur2 != NULL; prev = cur2, cur2 = cur2->next)

            if (is_equal_vars(cur, cur2)) {

                cur->mult += cur2->mult;
                prev->next = cur2->next;
                dtor_variable(cur2->vars);
                free(cur2);
                cur2 = prev;

            }
    clean_zero_elements(res);

    return PS_FORM_NO_ERROR;
}

static enum PS_FORM_ERROR monome_div(node_monome* res, const node_monome* term2) {
    if (res == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;

    if (term2->mult == 0)
        return PS_FORM_CANT_DIV;

    node_variable  dummy    = {res->vars, 0, 0};
    node_variable* prev     = &dummy,
                  *current  = res->vars,
                  *current2 = term2->vars;

    while (current2 != NULL && current != NULL) {

        if (current->var == current2->var) {
            if (current->degree == current2->degree) {

                prev->next    = current->next;
                current->next = NULL;
                dtor_variable(current);
                current       = prev;
                current2      = current2->next;

            } else if (current->degree > current2->degree) {
                current->degree -= current2->degree;
                current2         = current2->next;

            } else
                return PS_FORM_CANT_DIV;

        }

        prev    = current;
        current = current->next;
    }

    if (current2 != NULL)
        return PS_FORM_CANT_DIV;

    res->vars = dummy.next;
    res->mult = res->mult / term2->mult;

    return PS_FORM_NO_ERROR;
}
enum PS_FORM_ERROR ps_form_div(ps_form* res, const ps_form* term1, const ps_form* term2) {
    if (res == NULL || term1 == NULL || term2 == NULL)
        return PS_FORM_FORM_NULLPTR;
    if (term2->polynomial != term2->tail)
        return PS_FORM_CANT_DIV;

    enum PS_FORM_ERROR err = copy_ps_form(res, term1);
    if (err != PS_FORM_NO_ERROR)
        return err;

    for (node_monome* iter = res->polynomial; iter != NULL; iter = iter->next) {
        err = monome_div(iter, term2->polynomial);
        if (err != PS_FORM_NO_ERROR) {
            dtor_ps_form(res);
            return err;
        }
    }

    return PS_FORM_NO_ERROR;
}

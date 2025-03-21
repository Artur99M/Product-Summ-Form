#ifndef PS_FORM_OPERATORS_H
#define PS_FORM_OPERATORS_H
#include "ps_form.h"
enum PS_FORM_ERROR ps_form_add  (ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_sum  (ps_form* res, const ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_ded  (ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_sub  (ps_form* res, const ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_mul  (ps_form* res, const ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_div  (ps_form* res, const ps_form* term1, const ps_form* term2);
enum PS_FORM_ERROR ps_form_equal(bool*    res, const ps_form* term1, const ps_form* term2);
#endif

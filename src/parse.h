#ifndef TINYLISP_PARSE_H
#define TINYLISP_PARSE_H

#include "tinylisp.h"

LispObject lisp_parse(TinyLisp lisp, char* line, int* pos);

#endif
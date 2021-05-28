#ifndef TINYLISP_TINYLISP_H
#define TINYLISP_TINYLISP_H

#include "error.h"
#include "stack.h"
#include "object.h"

#define LISP_MAX_ERR_MSG_SIZE 1024

typedef struct TinyLisp_ *TinyLisp;

struct TinyLisp_
{
	error_t err_code;
	char err_msg[LISP_MAX_ERR_MSG_SIZE];
	LispStack stack;
};

TinyLisp lisp_new();
void lisp_free(TinyLisp lisp);
void lisp_clear_error(TinyLisp lisp);
void lisp_print_error(TinyLisp lisp);
void lisp_error_set(TinyLisp lisp, error_t err_code, char* format, ...);



#endif
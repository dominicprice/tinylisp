#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "tinylisp.h"

TinyLisp lisp_new()
{
	TinyLisp lisp = malloc(sizeof(struct TinyLisp_));
	if (lisp == NULL)
		return NULL;
	lisp->err_code = E_SUCCESS;
	lisp->err_msg[0] = '\0';
	lisp->stack = lisp_stack_new();
	if (lisp->stack == NULL) {
		free(lisp);
		return NULL;
	}
	return lisp;
}

void lisp_free(TinyLisp lisp)
{
	free(lisp->stack);
	free(lisp);
}

void lisp_clear_error(TinyLisp lisp)
{
	lisp->err_code = E_SUCCESS;
	lisp->err_msg[0] = '\0';
}

void lisp_print_error(TinyLisp lisp)
{
	printf("Error %d (%s)", lisp->err_code, errordesc[lisp->err_code].message);
	if (lisp->err_msg[0] == '\0')
		printf("\n");
	else
		printf(": %s\n", lisp->err_msg);
}

void lisp_error_set(TinyLisp lisp, error_t err_code, char* format, ...)
{
	lisp->err_code = err_code;
	if (format == NULL) {
		lisp->err_msg[0] = '\0';
	}
	else {
		va_list args;
		va_start(args, format);
		vsnprintf_s(lisp->err_msg, LISP_MAX_ERR_MSG_SIZE, LISP_MAX_ERR_MSG_SIZE, format, args);
		va_end(args);
	}
}
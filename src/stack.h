#ifndef TINYLISP_STACK_H
#define TINYLISP_STACK_H

#include "object.h"

#define LISP_MAX_STACK_SIZE 128

typedef struct LispStackFrame_ *LispStackFrame;
typedef struct LispStack_ *LispStack;

struct LispStackFrame_
{
	int size, capacity;
	char** keys;
	LispObject* vals;
};

LispStackFrame lisp_stackframe_new();
void lisp_stackframe_free(LispStackFrame frame);
LispObject lisp_stackframe_find(LispStackFrame frame, char* key);
error_t lisp_stackframe_set(LispStackFrame frame, char* key, LispObject val);
void lisp_stackframe_print(LispStackFrame frame);

struct LispStack_
{
	LispStackFrame frames[LISP_MAX_STACK_SIZE];
	int nframes;
};

LispStack lisp_stack_new();
void lisp_stack_free(LispStack stack);
LispObject lisp_stack_find(LispStack stack, char* key);
error_t lisp_stack_setlocal(LispStack stack, char* key, LispObject val);
error_t lisp_stack_setglobal(LispStack stack, char* key, LispObject val);
error_t lisp_stack_push(LispStack stack, LispStackFrame frame);
error_t lisp_stack_push_empty(LispStack stack);
void lisp_stack_pop(LispStack stack);

#endif
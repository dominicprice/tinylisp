#ifndef TINYLISP_BUILTINS_H
#define TINYLISP_BUILTINS_H

#include "tinylisp.h"
#include "object.h"
#include "stack.h"

#define LISP_BUILTIN_DEF(name) LispObject name(TinyLisp lisp, LispObject args)

//Takes two arguments, a value and a list, and returns a new list obtained by adding the value at the front of the list.
LISP_BUILTIN_DEF(construct);

//Takes a list and returns the first item in it, or nil if given nil.
LISP_BUILTIN_DEF(head);

//Takes a list and returns a new list containing all but the first item, or nil if given nil.
LISP_BUILTIN_DEF(tail);

//Takes two integers and returns the first minus the second.
LISP_BUILTIN_DEF(subtract);

// Takes two integers; returns 1 if the first is less than the second, 0 otherwise.
LISP_BUILTIN_DEF(lessthan);

//Takes two values of the same type (both integers, both lists, or both symbols); returns 1 if the 
//two are equal (or identical in every element), 0 otherwise. Testing builtins for equality is undefined
LISP_BUILTIN_DEF(equal);

// Takes one list, integer, or symbol, representing an expression, and evaluates it. 
// E.g. doing (v (q (c a b))) is the same as doing (c a b); (v 1) gives 1.
LISP_BUILTIN_DEF(eval);

// Takes one expression and returns it unevaluated. E.g., evaluating (1 2 3) gives an error 
//because it tries to call 1 as a function or macro, but (q (1 2 3)) returns the list (1 2 3).
//Evaluating a gives the value bound to the name a, but (q a) gives the name itself.
LISP_BUILTIN_DEF(quote);

//Takes three expressions: a condition, an iftrue expression, and an iffalse expression. Evaluates 
//the condition first. If the result is falsy (0 or nil), evaluates and returns the iffalse expression. 
//Otherwise, evaluates and returns the iftrue expression. Note that the expression that is not returned is never evaluated.
LISP_BUILTIN_DEF(ternary);

// Takes a symbol and an expression. Evaluates the expression and binds it to the given symbol treated as a
//name at global scope, then returns the symbol. Trying to redefine a name should fail (silently, with a message,
//or by crashing; the reference implementation displays an error message). Note: it is not necessary to quote the 
//name before passing it to d, though it is necessary to quote the expression if it's a list or symbol you don't want
//evaluated: e.g., (d x (q (1 2 3))).
LISP_BUILTIN_DEF(def);

#endif
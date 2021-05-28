#include "builtins.h"
#include "object.h"
#include "eval.h"
#include <stdarg.h>

#define ASSERT_ARGS(n) if (lisp_list_size(args) != n) { lisp_error_set(lisp, E_INDEX_ERROR, "Expected %d arguments, got %d", n, lisp_list_size(args)); return NULL;}
#define FUNCARG(name, idx) LispObject name = lisp_evaluate(lisp, lisp_list_at(args, idx))
#define MACROARG(name, idx) LispObject name = lisp_list_at(args, idx)

LISP_BUILTIN_DEF(construct)
{
	ASSERT_ARGS(2);
	FUNCARG(lhs, 0);
	if (lhs == NULL)
		return NULL;
	FUNCARG(rhs, 1);
	if (rhs == NULL) {
		lisp_object_free(lhs);
		return NULL;
	}
	if (rhs->type != T_LIST) {
		lisp_error_set(lisp, E_TYPE_ERROR, "Argument 2 required to be of type list");
		lisp_object_free(lhs);
		lisp_object_free(rhs);
		return NULL;
	}

	LispObject res = lisp_list_new_from_args(1, lisp_object_create_reference(lhs));
	for (int i = 0; i < lisp_list_size(rhs); ++i)
		lisp_list_push(res, lisp_object_create_reference(lisp_list_at(rhs, i)));
	lisp_object_free(lhs);
	lisp_object_free(rhs);
	return res;
}


LISP_BUILTIN_DEF(head)
{
	ASSERT_ARGS(1);
	FUNCARG(list, 0);
	if (list == NULL)
		return NULL;
	if (list->type != T_LIST) {
		lisp_error_set(lisp, E_TYPE_ERROR, "Argument 1 must be of type list");
		lisp_object_free(list);
		return NULL;
	}
	LispObject res = lisp_list_at(list, 0);
	res = lisp_object_create_reference(res);
	lisp_object_free(list);
	return res;
}

LISP_BUILTIN_DEF(tail)
{
	ASSERT_ARGS(1);
	FUNCARG(list, 0);
	if (list == NULL)
		return NULL;
	if (list->type != T_LIST) {
		lisp_error_set(lisp, E_TYPE_ERROR, "Argument 1 must be of type list");
		lisp_object_free(list);
		return NULL;
	}
	LispObject res = lisp_list_new();
	for (int i = 1; i < lisp_list_size(list); ++i)
		lisp_list_push(res, lisp_object_create_reference(lisp_list_at(list, i)));
	lisp_object_free(list);
	return res;
}


LISP_BUILTIN_DEF(subtract)
{
	ASSERT_ARGS(2);
	FUNCARG(x, 0);
	if (x == NULL)
		return NULL;
	if (x->type != T_INTEGER) {
		lisp_error_set(lisp, E_TYPE_ERROR, "Argument 1 must be of type integer");
		lisp_object_free(x);
		return NULL;
	}
	FUNCARG(y, 1);
	if (y == NULL)
		return NULL;
	if (y->type != T_INTEGER) {
		lisp_error_set(lisp, E_TYPE_ERROR, "Argument 2 must be of type integer");
		lisp_object_free(x);
		lisp_object_free(y);
		return NULL;
	}
	LispObject res = lisp_integer_new(lisp_integer_get(x) - lisp_integer_get(y));
	lisp_object_free(x);
	lisp_object_free(y);
	return res;
}


LISP_BUILTIN_DEF(lessthan)
{
	ASSERT_ARGS(2);
	FUNCARG(x, 0);
	if (x == NULL)
		return NULL;
	FUNCARG(y, 1);
	if (y == NULL) {
		lisp_object_free(x);
		return NULL;
	}
	LispObject res = lisp_integer_new(lisp_object_lessthan(x, y));
	lisp_object_free(x);
	lisp_object_free(y);
	return res;
}


LISP_BUILTIN_DEF(equal)
{
	ASSERT_ARGS(2);
	FUNCARG(x, 0);
	if (x == NULL)
		return NULL;
	FUNCARG(y, 1);
	if (y == NULL) {
		lisp_object_free(x);
		return NULL;
	}
	LispObject res = lisp_integer_new(lisp_object_equal(x, y));
	lisp_object_free(x);
	lisp_object_free(y);
	return res;
}


LISP_BUILTIN_DEF(eval)
{
	ASSERT_ARGS(1);
	FUNCARG(obj, 0);
	if (obj == NULL)
		return NULL;
	LispObject res = lisp_evaluate(lisp, obj);
	lisp_object_free(obj);
	return res;
}

LISP_BUILTIN_DEF(quote)
{
	ASSERT_ARGS(1);
	MACROARG(obj, 0);
	return lisp_object_create_reference(obj);
}

LISP_BUILTIN_DEF(ternary)
{
	ASSERT_ARGS(3);
	FUNCARG(pred, 0);
	if (pred == NULL)
		return NULL;

	if (lisp_object_is_nil(pred)) {
		FUNCARG(res, 2);
		return res;
	}
	else {
		FUNCARG(res, 1);
		return res;
	}
}

LISP_BUILTIN_DEF(def)
{
	ASSERT_ARGS(2);
	MACROARG(key, 0);
	FUNCARG(val, 1);
	if (val == NULL)
		return NULL;
	error_t err = lisp_stack_setglobal(lisp->stack, lisp_symbol_get(key), val);
	if (err != E_SUCCESS) {
		lisp_error_set(lisp, err, NULL);
		lisp_object_free(val);
		return NULL;
	}
	lisp_object_free(val);
	return lisp_object_create_reference(key);
}


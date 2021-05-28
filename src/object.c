#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "object.h"

//#define DEBUG

#ifdef DEBUG
#define VALIDATE_OBJECT(name) if (name == NULL || name->type < T_LIST || name->type >= T_SIZE) breakpoint()
void breakpoint()
{
	// break here
}
#else
#define VALIDATE_OBJECT(name)
#endif

struct type_desc_ typedesc[T_SIZE] = {
	{ T_LIST, "list" },
	{ T_INTEGER, "integer" },
	{ T_SYMBOL, "symbol" },
	{ T_BUILTIN, "builtin" }
};

LispObject lisp_object_new_(LispObjectType type)
{
	LispObject obj = malloc(sizeof(struct LispObject_));
	if (obj == NULL)
		return NULL;

	obj->refcount = 1;
	obj->type = type;

	return obj;
}

LispObject lisp_object_create_reference(LispObject other) {
	VALIDATE_OBJECT(other);
	++other->refcount;
	return other;
}

void lisp_object_free(LispObject obj)
{
	VALIDATE_OBJECT(obj);
	switch (obj->type) {
	case T_LIST:
		lisp_list_free(obj);
		break;
	case T_INTEGER:
		lisp_integer_free(obj);
		break;
	case T_SYMBOL:
		lisp_symbol_free(obj);
		break;
	case T_BUILTIN:
		lisp_builtin_free(obj);
		break;
	}
}

int lisp_object_equal(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	if (lhs->type != rhs->type)
		return 0;

	switch (lhs->type) {
	case T_LIST:
		return lisp_list_equal(lhs, rhs);
		break;
	case T_INTEGER:
		return lisp_integer_equal(lhs, rhs);
		break;
	case T_SYMBOL:
		return lisp_symbol_equal(lhs, rhs);
		break;
	case T_BUILTIN:
		return lisp_builtin_equal(lhs, rhs);
		break;
	default:
		return 0;
	}
}

int lisp_object_lessthan(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	if (lhs->type != rhs->type)
		return 0;

	switch (lhs->type) {
	case T_LIST:
		return lisp_list_lessthan(lhs, rhs);
		break;
	case T_INTEGER:
		return lisp_integer_lessthan(lhs, rhs);
		break;
	case T_SYMBOL:
		return lisp_symbol_lessthan(lhs, rhs);
		break;
	case T_BUILTIN:
		return lisp_builtin_lessthan(lhs, rhs);
		break;
	default:
		return 0;
	}
}

int lisp_object_is_nil(LispObject obj)
{
	VALIDATE_OBJECT(obj);
	if (obj->type == T_INTEGER)
		return lisp_integer_get(obj) == 0 ? 1 : 0;
	if (obj->type == T_LIST)
		return lisp_list_size(obj) == 0 ? 1 : 0;
	return 0;
}

void lisp_object_print(LispObject obj)
{
	VALIDATE_OBJECT(obj);
	if (obj == NULL) {
		printf("NULL");
		return;
	}
	switch (obj->type) {
	case T_LIST:
		lisp_list_print(obj);
		break;
	case T_INTEGER:
		lisp_integer_print(obj);
		break;
	case T_SYMBOL:
		lisp_symbol_print(obj);
		break;
	case T_BUILTIN:
		lisp_builtin_print(obj);
		break;
	default:
		printf("Unknown type");
	}
}

LispObject lisp_list_new()
{
	LispObject list = lisp_object_new_(T_LIST);
	if (list == NULL)
		return NULL;

	LispList data = malloc(sizeof(struct LispList_));
	if (data == NULL) {
		free(list);
		return NULL;
	}

	data->capacity = data->size = 0;
	data->data = NULL;

	list->data.l = data;
	return list;
}

LispObject lisp_list_new_from_args(int n, ...)
{
	LispObject* data = malloc(sizeof(LispObject) * n);
	if (data == NULL)
		return NULL;
	LispObject list = lisp_list_new();
	if (list == NULL) {
		free(data);
		return NULL;
	}

	list->data.l->capacity = list->data.l->size = n;
	list->data.l->data = data;
	
	va_list args;
	va_start(args, n);
	for (int i = 0; i < n; ++i) {
		LispObject obj = va_arg(args, LispObject);
		data[i] = obj;
	}
	va_end(args);

	return list;
}

LispObject lisp_list_copy(LispObject list)
{
	VALIDATE_OBJECT(list);
	LispObject res = lisp_list_new();
	int len = lisp_list_size(list);
	LispObject* data = malloc(sizeof(LispObject) * len);
	if (data == NULL)
		return NULL;
	for (int i = 0; i < len; ++i)
		data[i] = lisp_object_create_reference(lisp_list_at(list, i));
	res->data.l->capacity = res->data.l->size = len;
	res->data.l->data = data;
	return res;
}

LispObject lisp_list_copy_n(LispObject list, int start, int end)
{
	VALIDATE_OBJECT(list);
	LispObject res = lisp_list_new();
	int len = end - start;
	LispObject* data = malloc(sizeof(LispObject) * len);
	if (data == NULL)
		return NULL;
	for (int i = 0; i < len; ++i)
		data[i] = lisp_object_create_reference(lisp_list_at(list, start + i));
	res->data.l->capacity = res->data.l->size = len;
	res->data.l->data = data;
	return res;
}

void lisp_list_free(LispObject list)
{
	VALIDATE_OBJECT(list);
	--list->refcount;
	if (list->refcount != 0)
		return;

	for (int i = 0; i < lisp_list_size(list); ++i)
		lisp_object_free(lisp_list_at(list, i));
	free(list);
}

int lisp_list_equal(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	int len = lisp_list_size(lhs);
	if (len != lisp_list_size(rhs))
		return 0;

	for (int i = 0; i < len; ++i) {
		if (!lisp_object_equal(lisp_list_at(lhs, i), lisp_list_at(rhs, i)))
			return 0;
	}
	return 1;
}

int lisp_list_lessthan(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	int len = lisp_list_size(lhs);
	int rhslen = lisp_list_size(rhs);

	for (int i = 0; i < len && i < rhslen; ++i) {
		if (lisp_object_lessthan(lisp_list_at(lhs, i), lisp_list_at(rhs, i)))
			return 1;
		if (lisp_object_lessthan(lisp_list_at(rhs, i), lisp_list_at(lhs, i)))
			return 0;
	}
	return len < rhslen ? 1 : 0;
}

int lisp_list_size(LispObject list)
{
	VALIDATE_OBJECT(list);
	return list->data.l->size;
}

int lisp_list_capacity(LispObject list)
{
	VALIDATE_OBJECT(list);
	return list->data.l->capacity;
}

LispObject lisp_list_at(LispObject list, int n)
{
	VALIDATE_OBJECT(list);
	return list->data.l->data[n];
}

error_t lisp_list_push(LispObject list, LispObject val)
{
	VALIDATE_OBJECT(list);
	VALIDATE_OBJECT(val);
	if (lisp_list_capacity(list) == 0) {
		LispObject* data = malloc(sizeof(LispObject));
		if (data == NULL)
			return E_MEMORY_ERROR;
		++list->data.l->capacity;
		list->data.l->data = data;
	}
	else if (lisp_list_size(list) == lisp_list_capacity(list)) {
		int new_capacity = lisp_list_capacity(list) * 2;
		LispObject* data = realloc(list->data.l->data, sizeof(LispObject) * new_capacity);
		if (data == NULL)
			return E_MEMORY_ERROR;
		list->data.l->capacity = new_capacity;
		list->data.l->data = data;
	}
	list->data.l->data[list->data.l->size] = val;
	++list->data.l->size;

	return E_SUCCESS;
}

LispObject lisp_list_concat(LispObject list, LispObject other)
{
	VALIDATE_OBJECT(list);
	VALIDATE_OBJECT(other);
	LispObject res = lisp_list_copy(list);
	if (res == NULL)
		return NULL;
	int len = lisp_list_size(other);
	error_t err;
	for (int i = 0; i < len; ++i) {
		err = lisp_list_push(list, lisp_list_at(other, i));
		if (err != E_SUCCESS) {
			lisp_list_free(res);
			return NULL;
		}
	}
	return res;
}

LispObject lisp_list_head(LispObject list)
{
	VALIDATE_OBJECT(list);
	if (lisp_list_size(list) == 0)
		return lisp_list_new();
	return lisp_object_create_reference(lisp_list_at(list, 0));
}

LispObject lisp_list_tail(LispObject list)
{
	VALIDATE_OBJECT(list);
	int len = lisp_list_size(list);
	return lisp_list_copy_n(list, 1, len);
}

void lisp_list_print(LispObject list)
{
	VALIDATE_OBJECT(list);
	printf("(");
	int len = lisp_list_size(list);
	if (len > 0) {
		for (int i = 0; i < len - 1; ++i) {
			lisp_object_print(lisp_list_at(list, i));
			printf(" ");
		}
		lisp_object_print(lisp_list_at(list, len - 1));
	}
	printf(")");
}

LispObject lisp_symbol_new(char* val)
{
	return lisp_symbol_new_n(val, (int)strlen(val));
}

LispObject lisp_symbol_new_n(char* val, int len)
{
	LispObject symbol = lisp_object_new_(T_SYMBOL);
	if (symbol == NULL)
		return NULL;
	
	LispSymbol data = malloc(sizeof(struct LispSymbol_));
	if (data == NULL) {
		free(symbol);
		return NULL;
	}

	data->size = len;
	data->data = malloc(sizeof(char) * (len + 1));
	if (data->data == NULL) {
		free(data);
		free(symbol);
		return NULL;
	}
	memcpy(data->data, val, sizeof(char) * len);
	data->data[len] = '\0';

	symbol->data.s = data;
	return symbol;
}

void lisp_symbol_free(LispObject symbol)
{
	VALIDATE_OBJECT(symbol);
	--symbol->refcount;
	if (symbol->refcount > 0)
		return;

	free(symbol->data.s->data);
	free(symbol);
}

int lisp_symbol_equal(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return strcmp(lisp_symbol_get(lhs), lisp_symbol_get(rhs)) == 0 ? 1 : 0;
}
int lisp_symbol_lessthan(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return strcmp(lisp_symbol_get(lhs), lisp_symbol_get(rhs)) < 0 ? 1 : 0;
}

char* lisp_symbol_get(LispObject symbol)
{
	VALIDATE_OBJECT(symbol);
	return symbol->data.s->data;
}

void lisp_symbol_print(LispObject symbol)
{
	VALIDATE_OBJECT(symbol);
	printf("%s", lisp_symbol_get(symbol));
}

LispObject lisp_integer_new(int val)
{
	LispObject integer = lisp_object_new_(T_INTEGER);
	if (integer == NULL)
		return NULL;
	integer->data.i = val;
	return integer;
}

int lisp_integer_equal(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return lisp_integer_get(lhs) == lisp_integer_get(rhs);
}

int lisp_integer_lessthan(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return lisp_integer_get(lhs) < lisp_integer_get(rhs);
}

void lisp_integer_free(LispObject integer)
{
	VALIDATE_OBJECT(integer);
	--integer->refcount;
	if (integer->refcount > 0)
		return;

	free(integer);
}

int lisp_integer_get(LispObject integer)
{
	VALIDATE_OBJECT(integer);
	return integer->data.i;
}

void lisp_integer_print(LispObject integer)
{
	VALIDATE_OBJECT(integer);
	printf("%d", lisp_integer_get(integer));
}

LispObject lisp_builtin_new(LispBuiltin val)
{
	LispObject obj = lisp_object_new_(T_BUILTIN);
	obj->data.b = val;
	return obj;
}

void lisp_builtin_free(LispObject builtin)
{
	VALIDATE_OBJECT(builtin);
	--builtin->refcount;
	if (builtin->refcount > 0)
		return;

	free(builtin);
}

int lisp_builtin_equal(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return lisp_builtin_get(lhs) == lisp_builtin_get(rhs);
}

int lisp_builtin_lessthan(LispObject lhs, LispObject rhs)
{
	VALIDATE_OBJECT(lhs);
	VALIDATE_OBJECT(rhs);
	return lisp_builtin_get(lhs) < lisp_builtin_get(rhs);
}

LispBuiltin lisp_builtin_get(LispObject builtin)
{
	VALIDATE_OBJECT(builtin);
	return builtin->data.b;
}

void lisp_builtin_print(LispObject builtin)
{
	VALIDATE_OBJECT(builtin);
	printf("<builtin at %p>", lisp_builtin_get(builtin));
}
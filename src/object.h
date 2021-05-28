#ifndef LISP_OBJECT_H
#define LISP_OBJECT_H

#include "error.h"


typedef struct TinyLisp_ *TinyLisp;
typedef enum LispObjectType_ LispObjectType;
typedef struct LispList_ *LispList;
typedef struct LispSymbol_ *LispSymbol;
typedef struct LispObject_ *LispObject;
typedef struct LispStack_ *LispStack;
typedef int LispInteger;
typedef LispObject(*LispBuiltin)(TinyLisp, LispObject);

enum LispObjectType_
{
	T_LIST,
	T_INTEGER,
	T_SYMBOL,
	T_BUILTIN,
	T_SIZE
};

struct type_desc_
{
	int code;
	char* name;
};

extern struct type_desc_ typedesc[T_SIZE];

struct LispList_
{
	int size, capacity;
	LispObject* data;
};

struct LispSymbol_
{
	int size;
	char* data;
};

struct LispObject_
{
	union {
		LispList l;
		LispSymbol s;
		LispInteger i;
		LispBuiltin b;
	} data;
	LispObjectType type;
	int refcount;
};

// malloc a new LispObject_ struct and initialise type and refcount. returns NULL on error.
LispObject lisp_object_new_(LispObjectType type);
// Increase refount of obj and return; never returns NULL
LispObject lisp_object_create_reference(LispObject obj);
// delegate to lisp_*_free based on type
void lisp_object_free(LispObject obj);
// delegate to lisp_*_equal based on type, return 0 for different types
int lisp_object_equal(LispObject lhs, LispObject rhs);
// delegate to lisp_*_lessthan based on type, return 0 for different types
int lisp_object_lessthan(LispObject lhs, LispObject rhs);
// return 1 if object is an empty list of 0, else returns 0
int lisp_object_is_nil(LispObject obj);
// delegate to lisp_*_print based on type
void lisp_object_print(LispObject obj);

// malloc a new empty list
LispObject lisp_list_new();
// malloc a list and initialise with the 'nvals' vals given 
LispObject lisp_list_new_from_args(int nvals, ...);
// malloc a new list with new references to the objects in list
LispObject lisp_list_copy(LispObject list);
// malloc a new list with new referencs to the objects in the sublist (start, end)
LispObject lisp_list_copy_n(LispObject list, int start, int end);
// decrease refcount of list; if recount is then 0 then call lisp_object_free on all items and free memory for list
void lisp_list_free(LispObject list);
// compare element-wise for equality
int lisp_list_equal(LispObject lhs, LispObject rhs);
// compare element-wise for lessthan
int lisp_list_lessthan(LispObject lhs, LispObject rhs);
// number of elements in list
int lisp_list_size(LispObject list);
// number of elements space is reserved for
int lisp_list_capacity(LispObject list);
// borrowed reference to the element at position n
LispObject lisp_list_at(LispObject list, int n);
// push an element to the end of the list modifying it; expands as needed
error_t lisp_list_push(LispObject list, LispObject val);
// return a new list consisting of new references to the elements in list and other
LispObject lisp_list_concat(LispObject list, LispObject other);
// return a new reference to the element at the front of the list
LispObject lisp_list_head(LispObject list);
// return a new list containing new references to the elements in the sublist (1, ...)
LispObject lisp_list_tail(LispObject list);
// print the list
void lisp_list_print(LispObject list);

// malloc a new symbol with value val
LispObject lisp_symbol_new(char* val);
// malloc a new symbol with the substring of val starting at 0 and spanning len chars
LispObject lisp_symbol_new_n(char* val, int len);
// decref symbol; if refcount is then 0 free all memory associated with it
void lisp_symbol_free(LispObject symbol);
// compare string values for equality
int lisp_symbol_equal(LispObject lhs, LispObject rhs);
// return 1 if lhs is lexicographically less than rhs, else 0
int lisp_symbol_lessthan(LispObject lhs, LispObject rhs);
// return a borrowed reference to the string value of symbol
char* lisp_symbol_get(LispObject symbol);
// print the symbol
void lisp_symbol_print(LispObject symbol);

// malloc a new integer with value val
LispObject lisp_integer_new(int val);
// decref object; if refcount is then 0 free all memory associated with it
void lisp_integer_free(LispObject integer);
// compare integer value for equality
int lisp_integer_equal(LispObject lhs, LispObject rhs);
// return lhs < rhs
int lisp_integer_lessthan(LispObject lhs, LispObject rhs);
// return integer value 
int lisp_integer_get(LispObject integer);
// print the integer
void lisp_integer_print(LispObject integer);

// malloc a new lispobject containing the builtin val
LispObject lisp_builtin_new(LispBuiltin val);
// compare function pointers for equality
int lisp_builtin_equal(LispObject lhs, LispObject rhs);
// return lhs < rhs
int lisp_builtin_lessthan(LispObject lhs, LispObject rhs);
// return the function pointer
LispBuiltin lisp_builtin_get(LispObject builtin);
// decref object; if refcount is then 0 free all memory associated with it
void lisp_builtin_free(LispObject builtin);
// print the builtins address
void lisp_builtin_print(LispObject builtin);

#endif
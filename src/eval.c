#include <stdlib.h>

#include "stack.h"
#include "eval.h"

//#define DEBUG
#ifdef DEBUG
#define DEBUGPRINT(obj) { printf("%*c(DEBUG)", tabsize, ' '); if (obj == NULL) { printf("NULL\n"); } else { lisp_object_print(obj); printf("\n"); } tabsize -= 2; }
int tabsize = 0;
#else
#define DEBUGPRINT(obj)
#endif


LispObject lisp_evaluate(TinyLisp lisp, LispObject obj)
{
#ifdef DEBUG
	tabsize += 2;
	printf("%*c(DEBUG) Evaluating ", tabsize, ' ');
	lisp_object_print(obj);
	if (lisp->stack->nframes > 1) {
		printf(" with stackframe ");
		lisp_stackframe_print(lisp->stack->frames[lisp->stack->nframes - 1]);
	}
	printf("\n");
#endif
	if (obj->type == T_INTEGER) {
		// Integer evaluates to itself
		LispObject res = lisp_object_create_reference(obj);
		DEBUGPRINT(res);
		return res;
	}
	else if (obj->type == T_SYMBOL) {
		// Symbol evaluates to its value in the stack
		LispObject res = lisp_stack_find(lisp->stack, lisp_symbol_get(obj));
		if (res == NULL) 
			lisp_error_set(lisp, E_UNDEFINED_NAME, "Symbol %s not in scope", lisp_symbol_get(obj));
		DEBUGPRINT(res);
		return lisp_object_create_reference(res);
	}
	else if (obj->type == T_BUILTIN) {
		// Builtin evaluates to itself
		LispObject res = lisp_object_create_reference(obj);
		DEBUGPRINT(res);
		return res;
	}
	else if (obj->type == T_LIST) {
		// Empty list evaluates to itself
		if (lisp_list_size(obj) == 0) {
			LispObject res = lisp_object_create_reference(obj);
			DEBUGPRINT(res);
			return res;
		}

		// Treats head as function called with tail as arguments
		LispObject func;
		func = lisp_evaluate(lisp, lisp_list_at(obj, 0));
		if (func == NULL) {
			DEBUGPRINT(func);
			return NULL;
		}

		if (func->type == T_BUILTIN) {
			// For builtins, deference the function pointer
			LispObject res = lisp_builtin_get(func)(lisp, lisp_list_tail(obj));
			DEBUGPRINT(res);
			return res;
		}
		else if (func->type == T_LIST) {
			// For lists, there is an optional empty list as the first element to signify a macro,
			// then a list of parameter names (or a symbol which will get assigned all the paramters
			// in a list) followed by the body. 
			int is_macro = (lisp_list_at(func, 0)->type == T_LIST && lisp_list_size(lisp_list_at(func, 0)) == 0);
			if (is_macro) {
				// Associate the arguments with the list of parameter names
				LispStackFrame frame = lisp_stackframe_new();
				LispObject arg_keys = lisp_list_at(func, 1);
				if (arg_keys->type == T_LIST) {
					int len = lisp_list_size(arg_keys);
					for (int i = 0; i < len; ++i) {
						error_t err = lisp_stackframe_set(frame, lisp_symbol_get(lisp_list_at(arg_keys, i)), lisp_list_at(obj, i + 1));
						if (err != E_SUCCESS) {
							lisp_error_set(lisp, err, NULL);
							lisp_stackframe_free(frame);
							DEBUGPRINT(NULL);
							return NULL;
						}
					}
				}
				else {
					error_t err = lisp_stackframe_set(frame, lisp_symbol_get(arg_keys), lisp_list_tail(obj));
					if (err != E_SUCCESS) {
						lisp_error_set(lisp, err, NULL);
						lisp_stackframe_free(frame);
						DEBUGPRINT(NULL);
						return NULL;
					}
				}
				lisp_stack_push(lisp->stack, frame);
				LispObject res = lisp_evaluate(lisp, lisp_list_at(func, 2));
				lisp_stack_pop(lisp->stack);
				DEBUGPRINT(NULL);
				return res;
			}
			else {
				// Associate the evaluated arguments with the list of parameter names
				LispStackFrame frame = lisp_stackframe_new();
				LispObject arg_keys = lisp_list_at(func, 0);
				if (arg_keys->type == T_LIST) {
					int len = lisp_list_size(arg_keys);
					for (int i = 0; i < len; ++i) {
						LispObject val = lisp_evaluate(lisp, lisp_list_at(obj, i + 1));
						if (val == NULL) {
							lisp_stackframe_free(frame);
							DEBUGPRINT(NULL);
							return NULL;
						}
						error_t err = lisp_stackframe_set(frame, lisp_symbol_get(lisp_list_at(arg_keys, i)), val);
						if (err != E_SUCCESS) {
							lisp_error_set(lisp, err, NULL);
							lisp_stackframe_free(frame);
							DEBUGPRINT(NULL);
							return NULL;
						}
					}
				}
				else {
					LispObject arglist = lisp_list_new();
					int len = lisp_list_size(obj);
					for (int i = 1; i < len; ++i) {
						LispObject val = lisp_evaluate(lisp, lisp_list_at(obj, i));
						if (val == NULL) {
							lisp_stackframe_free(frame);
							DEBUGPRINT(NULL);
							return NULL;
						}
						lisp_list_push(arglist, val);
					}
					error_t err = lisp_stackframe_set(frame, lisp_symbol_get(arg_keys), arglist);
					if (err != E_SUCCESS) {
						lisp_error_set(lisp, err, NULL);
						lisp_stackframe_free(frame);
						lisp_object_free(arglist);
						DEBUGPRINT(NULL);
						return NULL;
					}
				}
				lisp_stack_push(lisp->stack, frame);
				LispObject res = lisp_evaluate(lisp, lisp_list_at(func, 1));
				lisp_stack_pop(lisp->stack);
				DEBUGPRINT(res);
				return res;
			}
		}
		else {
			lisp_error_set(lisp, E_TYPE_ERROR, "Expected a callable type, got %s", typedesc[func->type].name);
			DEBUGPRINT(NULL);
			return NULL;
		}
	}
	else {
		lisp_error_set(lisp, E_EVALUATION_ERROR, "Received unknown type %d", obj->type);
		DEBUGPRINT(NULL);
		return NULL;
	}
}
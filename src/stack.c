#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "stack.h"
#include "builtins.h"

// Returns the highest position of k in keys for which k <= key
int bfind_key(int n_keys, char** keys, char* key)
{
	int low = 0;
	int high = n_keys;
	int mid = n_keys / 2;
	while (low != mid) {
		int comp = strcmp(keys[mid], key);
		if (comp == 0)
			return mid;
		if (comp < 0)
			low = mid;
		else
			high = mid;
		mid = (high + low) / 2;
	}
	return mid;
}

LispStackFrame lisp_stackframe_new()
{
	LispStackFrame frame = malloc(sizeof(struct LispStackFrame_));
	if (frame == NULL)
		return NULL;
	frame->size = frame->capacity = 0;
	frame->keys = NULL;
	frame->vals = NULL;
	return frame;
}

void lisp_stackframe_free(LispStackFrame frame)
{
	for (int i = 0; i < frame->size; ++i) {
		free(frame->keys[i]);
		lisp_object_free(frame->vals[i]);
	}
	free(frame);
}

error_t lisp_stackframe_grow(LispStackFrame frame)
{
	if (frame->capacity == 0) {
		frame->keys = malloc(sizeof(char*));
		if (frame->keys == NULL)
			return E_MEMORY_ERROR;
		frame->vals = malloc(sizeof(LispObject));
		if (frame->vals == NULL) {
			free(frame->keys);
			frame->keys = NULL;
			return E_MEMORY_ERROR;
		}
		frame->capacity = 1;
	}
	else {
		int new_capacity = frame->capacity * 2;

		char** new_keys = realloc(frame->keys, sizeof(char*) * new_capacity);
		if (!new_keys)
			return E_MEMORY_ERROR;
		frame->keys = new_keys;

		LispObject* new_vals = realloc(frame->vals, sizeof(LispObject) * new_capacity);
		if (!new_vals)
			return E_MEMORY_ERROR;
		frame->vals = new_vals;
		frame->capacity = new_capacity;
	}

	return E_SUCCESS;
}

error_t lisp_stackframe_grow_if_full(LispStackFrame frame)
{
	if (frame->size == frame->capacity)
		return lisp_stackframe_grow(frame);
	return E_SUCCESS;
}

LispObject lisp_stackframe_find(LispStackFrame frame, char* key)
{
	if (frame->size == 0)
		return NULL;

	int pos = bfind_key(frame->size, frame->keys, key);
	if (strcmp(frame->keys[pos], key) == 0)
		return frame->vals[pos];
	return NULL;
}

error_t lisp_stackframe_set(LispStackFrame frame, char* key, LispObject val)
{
	error_t err;
	if (frame->size == 0) {
		err = lisp_stackframe_grow_if_full(frame);
		if (err != E_SUCCESS)
			return err;
		frame->keys[0] = _strdup(key);
		if (frame->keys[0] == NULL)
			return E_MEMORY_ERROR;
		frame->vals[0] = val;
		frame->size = 1;
		return E_SUCCESS;
	}

	int pos = bfind_key(frame->size, frame->keys, key);
	if (strcmp(frame->keys[pos], key) == 0) {
		return E_NAME_ALREADY_SET;
	}
	else {
		error_t err = lisp_stackframe_grow_if_full(frame);
		if (err != E_SUCCESS)
			return err;
		char* new_key = _strdup(key);
		if (new_key == NULL)
			return E_MEMORY_ERROR;
		for (int i = frame->size - 1; i > pos; --i) {
			frame->keys[i + 1] = frame->keys[i];
			frame->vals[i + 1] = frame->vals[i];
		}
		frame->keys[pos + 1] = new_key;
		frame->vals[pos + 1] = val;
		++frame->size;
	}
	return E_SUCCESS;
}

void lisp_stackframe_print(LispStackFrame frame)
{
	printf("{ ");
	if (frame->size > 0) {
		for (int i = 0; i < frame->size - 1; ++i) {
			printf("%s: ", frame->keys[i]);
			lisp_object_print(frame->vals[i]);
			printf(", ");
		}
		printf("%s: ", frame->keys[frame->size - 1]);
		lisp_object_print(frame->vals[frame->size - 1]);
	}
	printf(" }");
}

LispStack lisp_stack_new()
{
	LispStack stack = malloc(sizeof(struct LispStack_));
	if (stack == NULL)
		return NULL;
	stack->nframes = 1;
	stack->frames[0] = lisp_stackframe_new();
	if (stack->frames[0] == NULL) {
		lisp_stack_free(stack);
		return NULL;
	}

	// Insert builtins to global namespace
	LispStackFrame globals = stack->frames[0];
	lisp_stackframe_set(globals, "c", lisp_builtin_new(construct));
	lisp_stackframe_set(globals, "h", lisp_builtin_new(head));
	lisp_stackframe_set(globals, "t", lisp_builtin_new(tail));
	lisp_stackframe_set(globals, "s", lisp_builtin_new(subtract));
	lisp_stackframe_set(globals, "l", lisp_builtin_new(lessthan));
	lisp_stackframe_set(globals, "e", lisp_builtin_new(equal));
	lisp_stackframe_set(globals, "v", lisp_builtin_new(eval));
	lisp_stackframe_set(globals, "q", lisp_builtin_new(quote));
	lisp_stackframe_set(globals, "i", lisp_builtin_new(ternary));
	lisp_stackframe_set(globals, "d", lisp_builtin_new(def));

	return stack;
}

void lisp_stack_free(LispStack stack)
{
	for (int i = 0; i < stack->nframes; ++i)
		free(stack->frames[i]);
	free(stack);
}

LispObject lisp_stack_find(LispStack stack, char* key)
{
	LispObject res = lisp_stackframe_find(stack->frames[stack->nframes - 1], key);
	if (res != NULL)
		return res;
	return lisp_stackframe_find(stack->frames[0], key);
}

error_t lisp_stack_setlocal(LispStack stack, char* key, LispObject val)
{
	return lisp_stackframe_set(stack->frames[stack->nframes - 1], key, lisp_object_create_reference(val));
}

error_t lisp_stack_setglobal(LispStack stack, char* key,LispObject val)
{
	return lisp_stackframe_set(stack->frames[0], key, lisp_object_create_reference(val));
}

error_t lisp_stack_push(LispStack stack, LispStackFrame frame)
{
	if (stack->nframes == LISP_MAX_STACK_SIZE)
		return E_STACK_OVERFLOW;
	stack->frames[stack->nframes] = frame;
	++stack->nframes;
	return E_SUCCESS;
}

error_t lisp_stack_push_empty(LispStack stack)
{
	if (stack->nframes == LISP_MAX_STACK_SIZE)
		return E_STACK_OVERFLOW;
	stack->frames[stack->nframes] = lisp_stackframe_new();
	if (stack->frames[stack->nframes] == NULL)
		return E_MEMORY_ERROR;
	++stack->nframes;
	return E_SUCCESS;
}

void lisp_stack_pop(LispStack stack)
{
	--stack->nframes;
	lisp_stackframe_free(stack->frames[stack->nframes]);
}

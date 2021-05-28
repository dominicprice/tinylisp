#include <stdlib.h>

#include "parse.h"


LispObject lisp_parse(TinyLisp lisp, char* line, int* pos)
{
	while (line[*pos] != '\0' && isspace(line[*pos]))
		++(*pos);

	if (line[*pos] == '\0') {
		lisp_error_set(lisp, E_NO_INPUT, NULL);
		return NULL;
	}
	else if (line[*pos] == '(') {
		LispObject res = lisp_list_new();
		++(*pos);
		while (line[*pos] != '\0') {
			if (isspace(line[*pos])) {
				++(*pos);
			}
			else if (line[*pos] == ')') {
				++(*pos);
				return res;
			}
			else {
				LispObject val = lisp_parse(lisp, line, pos);
				if (val == NULL) {
					lisp_object_free(res);
					return NULL;
				}
				error_t err = lisp_list_push(res, val);
				if (err != E_SUCCESS) {
					lisp_list_free(res);
					lisp_error_set(lisp, err, "Could not construct list");
					return NULL;
				}
			}
		}
		lisp_list_free(res);
		lisp_error_set(lisp, E_UNEXPECTED_EOF, NULL);
		return NULL;
	}
	else if (line[0] == ')') {
		lisp_error_set(lisp, E_SYNTAX_ERROR, ") found with no bracket group to end");
		return NULL;
	}
	else {
		int is_number = 1;
		int end = *pos;
		while (line[end] > 0x20 && line[end] <= 0x7E && line[end] != '(' && line[end] != ')') {
			if (line[end] < 0x30 || line[end] > 0x39)
				is_number = 0;
			++end;
		}

		LispObject res;
		if (is_number)
			res = lisp_integer_new(atoi(line + *pos));
		else
			res = lisp_symbol_new_n(line + *pos, end - *pos);
		*pos = end;
		return res;
	}
}
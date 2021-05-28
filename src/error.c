#include "error.h"

struct error_desc_ errordesc[E_SIZE] = {
	{ E_SUCCESS, "No Error" },
	{ E_NAME_ALREADY_SET, "Name already set" },
	{ E_MEMORY_ERROR, "Ran out of memory" },
	{ E_STACK_OVERFLOW, "Stack overflow" },
	{ E_SYNTAX_ERROR, "Syntax error" },
	{ E_UNEXPECTED_EOF, "Unexpected EOF" },
	{ E_UNDEFINED_NAME, "Undefined name" },
	{ E_EVALUATION_ERROR, "Evaluation error" },
	{ E_TYPE_ERROR, "Type error"},
	{ E_NO_INPUT, "No input received" },
	{ E_INDEX_ERROR, "List index out of range" }
};

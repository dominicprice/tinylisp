#ifndef TINYLISP_ERROR_H
#define TINYLISP_ERROR_H

enum error_t_
{
	E_SUCCESS,
	E_NAME_ALREADY_SET,
	E_MEMORY_ERROR,
	E_STACK_OVERFLOW,
	E_SYNTAX_ERROR,
	E_UNEXPECTED_EOF,
	E_UNDEFINED_NAME,
	E_EVALUATION_ERROR,
	E_TYPE_ERROR,
	E_NO_INPUT,
	E_INDEX_ERROR,
	E_SIZE
};

typedef enum error_t_ error_t;

struct error_desc_
{
	int code;
	char* message;
};

extern struct error_desc_ errordesc[E_SIZE];

#endif
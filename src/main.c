#include <stdio.h>
#include <string.h>
#include "tinylisp.h"
#include "parse.h"
#include "eval.h"

#define BUFFER_SIZE 1024

void help()
{
	printf("usage: tinylisp [--help|-h] [--nobanner|-q] scriptfile\n");
	printf("Builtin commands:\n");
	printf("(c)onstruct\tTakes two arguments, a value and a list, and returns a new list obtained by adding the value at the front of the list.\n");
}

int read_file(char* filename)
{
	FILE* input = NULL;
	int err = fopen_s(&input, filename, "r");
	if (err != 0) {
		printf("Could not open file %s", filename);
		return -1;
	}

	TinyLisp lisp = lisp_new();
	char buffer[BUFFER_SIZE];
	int collect_len = 0;

	for (;;) {
		if (!fgets(buffer + collect_len, BUFFER_SIZE - collect_len, input))
			return 1;
		collect_len = (int)strlen(buffer);
		int pos = 0;
		while (pos < collect_len) {
			LispObject obj = lisp_parse(lisp, buffer, &pos);
			if (obj != NULL) {
				collect_len = 0;
				LispObject eval = lisp_evaluate(lisp, obj);
				if (eval != NULL) {
					lisp_object_print(eval);
					printf("\n");
					lisp_object_free(eval);
				}
				else {
					lisp_print_error(lisp);
					lisp_clear_error(lisp);
				}
				lisp_object_free(obj);
			}
			else {
				if (lisp->err_code == E_UNEXPECTED_EOF) {
					lisp_clear_error(lisp);
					continue;
				}
				else if (lisp->err_code == E_NO_INPUT) {
					collect_len = 0;
					lisp_clear_error(lisp);
					continue;
				}
				else {
					lisp_print_error(lisp);
					return lisp->err_code;
				}
			}
		}
	}
	return 0;
}

int interact(int banner)
{
	if (banner) {
		printf(" _______ _____ __   _ __   __        _____ _______  _____\n");
		printf("    |      |   | \\  |   \\_/   |        |   |______ |_____]\n");
		printf("    |    __|__ |  \\_|    |    |_____ __|__ ______| |\n");
		printf("             Version 0.1, Copyright (C) Dominic Price 2021\n\n");
	}

	TinyLisp lisp = lisp_new();
	char buffer[BUFFER_SIZE];
	int collect_len = 0;

	for (;;) {
		printf(collect_len == 0 ? "> " : ". ");
		if (!fgets(buffer + collect_len, BUFFER_SIZE - collect_len, stdin))
			return 1;
		collect_len = (int)strlen(buffer);
		int pos = 0;
		while (pos < collect_len) {
			LispObject obj = lisp_parse(lisp, buffer, &pos);
			if (obj != NULL) {
				collect_len = 0;
				LispObject eval = lisp_evaluate(lisp, obj);
				if (eval != NULL) {
					lisp_object_print(eval);
					printf("\n");
					lisp_object_free(eval);
				}
				else {
					lisp_print_error(lisp);
					lisp_clear_error(lisp);
				}
				lisp_object_free(obj);
			}
			else {
				if (lisp->err_code == E_UNEXPECTED_EOF) {
					lisp_clear_error(lisp);
					continue;
				}
				else if (lisp->err_code == E_NO_INPUT) {
					collect_len = 0;
					lisp_clear_error(lisp);
					continue;
				}
				else {
					lisp_print_error(lisp);
					lisp_clear_error(lisp);
				}
			}
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	int quiet = 0;
	char* filename = NULL;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			help();
			return 0;
		}
		else if (strcmp(argv[i], "--nobanner") == 0 || strcmp(argv[i], "-q") == 0) {
			quiet = 1;
		}
		else {
			filename = argv[i];
		}
	}

	if (filename != NULL)
		return read_file(filename);
	else
		return interact(!quiet);
}
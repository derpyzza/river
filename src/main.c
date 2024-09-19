#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

typedef struct str_pos {
	int start, len, 
			line;
} str_pos;

int
main(void) {
	printf("Welcome to the river compiler!\n");
	char* code = " int  return  ( ) identifier 214134 'h' \"this is a string\" 434";
	token_array_s* tkn = tokenize(code);
	print_token_array(*tkn);
	free(tkn);
}

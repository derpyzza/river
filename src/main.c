#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

file_s *read_file(char* path) {
	file_s *out = malloc(sizeof(file_s));
	out->path = path;
	out->is_valid = 0;
	FILE *in = fopen(path, "rb");
	if (in == NULL) {
		printf("Error, could not read file %s\n", path);
		return out;
	}
	fseek(in, 0, SEEK_END); 
	out->size = ftell(in); 
	rewind(in); 

	out->data = malloc(out->size + 1);
	if (out->data == NULL) {
		printf("Error, could not allocate enough memory for file %s\n", path);
		fclose(in);
		return out;
	}

	size_t bufsread = fread(out->data, 1, out->size, in);
	if ( bufsread < out->size ) {
		printf("Error, could not read enough bytes from file %s\n", path);
		fclose(in);
		return out;
	}

	fclose(in);
	return out;
}


int
main(int argc, char** argv) {
	printf("Welcome to the river compiler!\n");

	if (argc < 2) {
		printf("No input files\n");
		exit(0);
	}

	for (int i = 1; i < argc; i++) {
		char* path = argv[i];
		printf("Compiling file %s...\n", path);
		file_s* source = read_file(path);
		// file_s source;
		// source.data = " int  return  ( ) identifier 214134 'h' \"this is a string\" 434";
		// source.size = strlen(source.data);
		token_array_s* tkn = tokenize(*source);
		print_token_array(*source, *tkn);
		free(tkn);
	}
	return 0;
}

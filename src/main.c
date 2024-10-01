#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "common.h"
#include "scanner.h"
#include "parser.h"
#include "codegen.h"

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
	out->data.size = ftell(in); 
	rewind(in); 

	out->data.string = malloc(out->data.size + 1);
	if (out->data.string == NULL) {
		printf("Error, could not allocate enough memory for file %s\n", path);
		fclose(in);
		return out;
	}

	size_t bufsread = fread(out->data.string, 1, out->data.size, in);
	if ( bufsread < out->data.size ) {
		printf("Error, could not read enough bytes from file %s\n", path);
		fclose(in);
		return out;
	}

	fclose(in);
	// everything went fine
	out->is_valid = 1;
	return out;
}

typedef struct path_s {
	char* path;
	char* ext;
} path_s;

path_s split_path(char* path) {
	char* _path = malloc(sizeof(path));
	int len = sizeof(path);
	int id = 0;
	char* c = path;
	for(c = path; *c != '.'; c++) {
		_path[id] = *c;
		id++;
	}
	_path[id] = 0;

	char* _ext = malloc(len - id * sizeof(char) + 1);
	strcpy(_ext, c);
	return (path_s){ .path = _path, .ext = _ext };
}


int
main(int argc, char** argv) {
	printf("Welcome to the river compiler!\n");

	// REPL MODE
	if (argc < 2) {
		int should_quit = 0;
		printf("No input files\n");
		int print_tokens = 0;
		while(!should_quit) {
			// arbitrarily sized buffer for now
			char* buf = malloc(2048);
			fputs("rvrcc>> ", stdout);
			fflush(stdout);
			if(!fgets(buf, 2048, stdin)){
				printf("ERROR: couldn't read string\n");
				exit(1);
			};

		  if ((strlen(buf) > 0) && (buf[strlen (buf) - 1] == '\n'))
        buf[strlen (buf) - 1] = '\0';

			if(!strcmp(buf, "quit")) {
				printf("k, bye then\n");
				exit(0);
			}

			// This is ugly, should change this.
			printf("bufsize: %li\n", strlen(buf));
			string_s source = (string_s) {.size = strlen(buf), .string = buf};
			token_array_s* tkn = tokenize(source);
			print_token_array(source, *tkn);

			// node_s *node = parse_tokens(tkn, source);
			// print_ast(*node);
			// printf("\n");
			free(tkn);
		}
		exit(0);
	}

	// Compile all the files given in turn
	for (int i = 1; i < argc; i++) {
		char* raw_path = argv[i];
		path_s path = split_path(raw_path);
		if (strcmp(path.ext, ".rvr")) {
			printf("Error: invalid filetype, expected \".rvr\", got \"%s\"\n", path.ext);
			continue;
		}
		printf("Compiling file %s...\n", raw_path);
		file_s* source = read_file(raw_path);
		if (!source->is_valid) {
			printf("ERROR: source file %s not valid\n", raw_path);
			exit(1);
		}

		FILE* out_file = fopen(strcat(path.path, ".c"), "wb");
		if (out_file == NULL) {
			printf("Error: Couldn't create output file %s.c\n", path.path);
			fclose(out_file);
			continue;
		}

		token_array_s* tkn = tokenize(source->data);
		// print_token_array(source->data, *tkn);

		node_s *node = parse_tokens(tkn, source->data);
		print_ast(*node);
		printf("\n");
		fprintf(out_file, "%s", codegen(node));
		fprintf(out_file, "\n");
		fclose(out_file);
		free(tkn);
	}
	return 0;
}

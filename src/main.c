#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "common.h"
#include "scanner.h"
#include "parser.h"

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
			// else if (!strcmp(buf, "tokens")) {
			// 	print_tokens = !print_tokens;
			// 	printf("tokens set to %s\n", print_tokens ? "true" : "false");
			// }

			// This is ugly, should change this.
			string_s source = (string_s) {.size = strlen(buf), .string = buf};
			token_array_s* tkn = tokenize(source);

			// print_token_array(source, *tkn);
			node_s *node = parse_tokens(tkn, source);
			print_ast(*node);
			printf("\n");
			// interpret(node);
			free(tkn);
		}
		exit(0);
	}

	// Compile all the files given in turn
	for (int i = 1; i < argc; i++) {
		char* path = argv[i];
		printf("Compiling file %s...\n", path);
		file_s* source = read_file(path);
		if (!source->is_valid) {
			printf("ERROR: source file %s not valid\n", path);
			exit(1);
		}

		token_array_s* tkn = tokenize(source->data);
		// print_ast(*parse_tokens(*tkn, source->data));
		free(tkn);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <libderp/derp.h>

// #include "memory.h"
#include "scanner.h"
#include "parser/parser.h"
// #include "codegen.h"

#define REPL_BUF_SIZE 2048

const char* rvr_ext_list[2] = {
	".rvr",
	".river"
};

int
main(int argc, char** argv) {	
	printf("Welcome to the river compiler!\n");

	// REPL MODE
	if (argc < 2) {
		printf("No input files, starting repl\n");
		char buf[REPL_BUF_SIZE];
		// 4 kilobytes of memory, just a random number
		d_arena mem = darena_init_alloc(4 * 1024);
		while(true) {
			fputs("rvrcc>> ", stdout);
			fflush(stdout);
			if(!fgets(buf, REPL_BUF_SIZE, stdin)){
				dlog_error("couldn't read string\n");
				exit(1);
			};

		  if ((strlen(buf) > 0) && (buf[strlen (buf) - 1] == '\n'))
	        buf[strlen (buf) - 1] = '\0';

			if(!strcmp(buf, "quit")) {
				printf("k, bye then\n");
				exit(0);
			}

			// This is ugly, should change this.
			dstr source = {.len = strlen(buf), .cptr = buf};
			dbuf_token* tkn = tokenize(source);
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
		printf("raw_path: %s\n", raw_path);
		dfilepath path = split_path(raw_path);
		printf("path: '%s', '%s'\n", path.path.cptr, path.ext.cptr);

		if (strcmp(path.ext.cptr, ".rvr")) {
			printf("Error: invalid filetype, expected \".rvr\", got \"%s\" instead.\n", path.ext.cptr);
			continue;
		}

		printf("Compiling file %s...\n", raw_path);

		dstr source = dfile_read(raw_path);
		if (source.cptr == NULL) {
			printf("ERROR, invalid source file: %s\n", raw_path);
			exit(1);
		}

		// FILE* out_file = fopen(strcat(path.path, ".c"), "wb");
		// if (out_file == NULL) {
		// 	printf("Error: Couldn't create output file %s.c\n", path.path);
		// 	fclose(out_file);
		// 	continue;
		// }

		dbuf_token* tkn = tokenize(source);
		print_token_array(source, *tkn);

		Node *node = parse_tokens(tkn, source);
		if(node != NULL) {
			printf("==== AST DUMP ====\n");
			print_ast(node, 0);
		}
		else printf("file had errors, compilation terminated\n");
		// printf("\n");
		// codegen(out_file, node->children->data[0]);
		// fclose(out_file);
		free(tkn);
		free(node);
	}
	return 0;
}

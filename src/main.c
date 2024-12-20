#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "utils.h"
#include "scanner.h"
#include "parser/parser.h"
#include "codegen.h"

struct File *read_file(char* path) {
	File *out = (File*)malloc(sizeof(File));
	out->path = path;
	out->is_valid = False;

	FILE *in = fopen(path, "rb");
	if (in == NULL) {
		printf("Error, could not read file %s\n", path);
		return out;
	}
	fseek(in, 0, SEEK_END); 
	out->data.len = ftell(in); 
	rewind(in); 

	out->data.c_ptr = (char*)malloc(out->data.len + 1);
	if (out->data.c_ptr == NULL) {
		printf("Error, could not allocate enough memory for file %s\n", path);
		fclose(in);
		return out;
	}

	size bufsread = fread(out->data.c_ptr, 1, out->data.len, in);
	if ( bufsread < out->data.len ) {
		printf("Error, could not read enough bytes from file %s\n", path);
		fclose(in);
		return out;
	}

	fclose(in);
	// everything went fine
	out->is_valid = True;
	return out;
}

typedef struct FilePath {
	char* path;
	char* ext;
} FilePath;

struct FilePath split_path(char* path) {
	size len = strlen(path);
	char* name = (char*)malloc(sizeof(char) * len);
	int id = 0;
	char* c = path;
	while(*c != '.') {
		name[id] = *c;
		id++;
		c++;
	}
	name[id] = '\0';

	printf("size is %li\n", (len - id) * sizeof(char) + 1);
	char* ext = (char*)malloc((len - id) * sizeof(char) + 1);
	if(ext != NULL) 
		strcpy(ext, c);
	else printf("Error: Could not malloc ext file\n"), exit(-1);
	return (FilePath){ .path = name, .ext = ext };
}

int
main(int argc, char** argv) {	
	printf("Welcome to the river compiler!\n");

	// REPL MODE
	if (argc < 2) {
		printf("No input files\n");
		while(True) {
			// arbitrarily sized buffer for now
			char* buf = (char*)malloc(2048);
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

			// printf("bufsize: %li\n", strlen(buf));
			// This is ugly, should change this.
			String source = (String) {.len = (int)strlen(buf), .c_ptr = buf};
			VecToken* tkn = tokenize(&source);
			print_token_array(&source, *tkn);

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
		FilePath path = split_path(raw_path);
		printf("path: '%s', '%s'\n", path.path, path.ext);

		if (strcmp(path.ext, ".rvr")) {
			printf("Error: invalid filetype, expected \".rvr\", got \"%s\" instead.\n", path.ext);
			continue;
		}

		printf("Compiling file %s...\n", raw_path);

		File* source = read_file(raw_path);
		if (!source->is_valid) {
			printf("ERROR, invalid source file: %s\n", raw_path);
			exit(1);
		}

		FILE* out_file = fopen(strcat(path.path, ".c"), "wb");
		if (out_file == NULL) {
			printf("Error: Couldn't create output file %s.c\n", path.path);
			fclose(out_file);
			continue;
		}

		VecToken* tkn = tokenize(&source->data);
		print_token_array(&source->data, *tkn);

		struct Node *node = parse_tokens(tkn, source);
		if(node != NULL) {
			printf("==== AST DUMP ====\n");
			print_ast(node, 0);
		}
		else printf("file had errors, compilation terminated\n");
		printf("\n");
		codegen(out_file, node->children->data[0]);
		fclose(out_file);
		free(tkn);
	}
	return 0;
}

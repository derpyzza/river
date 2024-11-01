
#include "parser.h"
#include "../scanner.h"
#include "parser_utils.h"
#include "../utils.h"
#include <stdio.h>

#include "../diagnostics.h"

void init_parser(File *source, TokenArray *tokens) {
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
	parser.had_error = 0;
}

Token *current_tok(void) { return &parser.tokens->token_list[parser.current]; }
int current(void) { return parser.current; }

void report_error(ParseError *error) {
	printf("[%sCOMPILER_DEBUG%s]: error emitted from <%s:%i>\n", 
	      TERM_LRED,
       	TERM_RESET, 
				error->debug_file,
				error->debug_line);

	if ( error->is_tok ) {
		printf("%s:%i:%i: ERROR Unexpected token; expected '%s', got '%.*s' (%s)\n",
			parser.source->path,
			parser.tokens->token_list[error->got].line,
			parser.tokens->token_list[error->got].chr_index,
			token_to_str(error->type),
			(int)parser.tokens->token_list[error->got].source.len,
			parser.tokens->token_list[error->got].source.c_ptr,
			token_to_str(parser.tokens->token_list[error->got].type));
	} else {
		printf("%s:%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
			parser.source->path,
			parser.tokens->token_list[error->got].line,
			parser.tokens->token_list[error->got].chr_index,
			token_to_str(error->type),
			(int)parser.tokens->token_list[error->got].source.len,
			parser.tokens->token_list[error->got].source.c_ptr,
			token_to_str(parser.tokens->token_list[error->got].type));
	}
}

int check_errors(void) {
	if (parser.had_error) {
		for (int i = 0; i < parser.errors->current; i++) {
			report_error((struct ParseError*)parser.errors->data[i]);
		}
		return 1;
	} else return 0;
}


void consume(void) { parser.current++; }

struct Token peek(void) { return peek_n(1); }

struct Token peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

struct Token prev(void) { return prev_n(1); }

struct Token prev_n(int offset)
{
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->token_list[parser.current - offset];
}

int check_next (int expected) { 
	// printf("next is: %s\n", token_to_str(peek().type));
	return check_next_n(1, expected);
}

int check_type_token(void) {
	TokenType next = peek().type;
	if ((next >= T_UBYTE && next <= T_BOOL) || (next == T_IDENTIFIER)) {
		return 1;	
	}

	return 0;
}

int check_next_cat ( int cat ) {
	return (peek().cat == cat); 
}

int check_next_n(int offset, int expected) { 
	// printf("T: %i\n", peek_n(offset).type == expected);
	return (peek_n(offset).type == expected); 
}

int match_type_token(void) {
	TokenType next = peek().type;
	if(check_type_token()) {
		consume();
		return next;
	}
	return 0;
}

int match_cat(int cat) {
	if ( tok_to_cat(peek().type) == cat ) { 
		consume();
		return 1; 
	} 
	return 0;
}

int match_range(int start, int end) 
{
	if (peek().type >= start && peek().type <= end) {
		consume();
		return 1;
	}
	return 0;
}

int match( int expected) {
	if (peek().type == expected) {
		consume();
		return 1;
	}
	return 0;
}

// int match_or_err(token_type expected)
// {
// 	if (check_next(expected)) {
// 		consume();
// 		return 1;
// 	}
// 	error_unexpected_tok(peek(), expected);
// 	return 0;
// }

void error_unexpected( int got, int expected, int is_tok, const char * file, const int line ) {
	if (!parser.had_error) {
		parser.had_error = 1;
		parser.errors = vec_of(struct ParseError*, 8);
	}
	struct ParseError *err = malloc(sizeof(struct ParseError));
	err->got = got;
	err->debug_file = file;
	err->debug_line = line;
	err->is_tok = is_tok;
	vec_push(parser.errors, err);
}

// void error_unexpected_cat( Token *got, int expected, const char * file, const int line ) {
// 	err_unexpected( got, expected, 0, file, line ); 
// }

// void error_unexpected_tok( Token *got, int expected, const char * file, const int line ) {
// 	err_unexpected( got, expected, 1, file, line); 
// }

struct Token *token_at(long id)
{
	if (id >= parser.tokens->current_token ) {
		return NULL;
	}
	return &parser.tokens->token_list[id];
}

struct Literal *cur_lit() {
	return lit_at(parser.current);
}

struct Literal *lit_at(long tok_id)
{
	if (tok_id >= parser.tokens->current_token) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return NULL;
	}
	int lit_id = token_at(tok_id)->literal_id;	
	return &parser.tokens->literal_list[lit_id];
}

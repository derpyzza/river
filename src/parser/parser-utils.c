#include "parser.h"
#include "parser-internal.h"

#include "../diagnostics.h"

void init_parser(File *source, VecToken *tokens) {
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
	parser.had_error = false;
	parser.errors = NULL;
}

void consume(void) { 
	parser.current++;
}

size current(void) {
	 return parser.current; 
}

Token current_tok(void) {
	return parser.tokens->data[parser.current]; 
}

String * cur_tok_span(void) {
	return &parser.tokens->data[parser.current].span;
}

Token *token_at(size id) {
	if (id >= parser.tokens->current ) {
		return NULL;
	}
	return &parser.tokens->data[id];
}

struct Token peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current ) {
		return token_eof();
	}
	return parser.tokens->data[parser.current + offset];
}

struct Token peek(void) { 
	return peek_n(1); 
}

struct Token prev_n(int offset) {
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->data[parser.current - offset];
}

struct Token prev(void) { 
	return prev_n(1);
}

int check_next_n(int offset, int expected) { 
	// printf("T: %i\n", peek_n(offset).type == expected);
	return (peek_n(offset).type == expected); 
}

int check_next (int expected) { 
	// printf("next is: %s\n", token_to_str(peek().type));
	return check_next_n(1, expected);
}

int check_next_cat ( int cat ) {
	return (peek().cat == cat); 
}

void error_unexpected( int got, int expected, bool is_tok, const char * file, const int line ) {
	if (!parser.had_error) {
		parser.had_error = 1;
		parser.errors = new_vec_ParseErrors(8);
	}
	struct ParseError *err = (ParseError*)malloc(sizeof(struct ParseError));
	err->type = expected;
	err->got = got;
	err->debug_file = file;
	err->debug_line = line;
	err->is_tok = is_tok;
	vec_push_ParseErrors(parser.errors, *err);
}

void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line) {
	error_unexpected(parser.current + 1, expected, is_expected_tok, file, line);
	if (is_expected_tok)
		while(!check_next(T_EOF) && !check_next(until)) consume();
	else
		while(!check_next(T_EOF) && !check_next_cat(until)) consume();
}

void report_error(ParseError *error) {
	printf("[%sCOMPILER_DEBUG%s]: error emitted from <%s:%i>\n", 
	      TERM_LRED,
       	TERM_RESET, 
				error->debug_file,
				error->debug_line);

	if ( error->is_tok ) {
		printf("%s:%i:%i: ERROR Unexpected token; expected '%s', got '%.*s' (%s)\n",
			parser.source->path,
			(int)parser.tokens->data[error->got].line,
			parser.tokens->data[error->got].chr_index,
			tok_to_str((TokenTag)error->type),
			(int)parser.tokens->data[error->got].span.len,
			parser.tokens->data[error->got].span.c_ptr,
			tok_to_str(parser.tokens->data[error->got].type));
	} else {
		printf("%s:%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
			parser.source->path,
			(int)parser.tokens->data[error->got].line,
			parser.tokens->data[error->got].chr_index,
			cat_to_str((TokCat)error->type),
			(int)parser.tokens->data[error->got].span.len,
			parser.tokens->data[error->got].span.c_ptr,
			tok_to_str(parser.tokens->data[error->got].type));
	}
}

bool check_errors(void) {
	if (parser.had_error) {
		for (int i = 0; i < parser.errors->current; i++) {
			report_error(&parser.errors->data[i]);
		}
		return true;
	} else return false;
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

int match( TokenTag expected) {
	if (peek().type == expected) {
		consume();
		return 1;
	}
	return 0;
}

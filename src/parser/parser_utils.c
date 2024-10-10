
#include "parser.h"
#include "../scanner.h"
#include "parser_utils.h"
#include "../utils.h"
#include <stdio.h>

struct parser {
	int current;
	file_s source; // current file being parsed
	token_array_s *tokens;
	int had_error;
	struct Vec* errors;
};

struct parser parser;

void init_parser(file_s source, token_array_s *tokens) {
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
	parser.had_error = 0;
}

token_s current_tok(void) { return parser.tokens->token_list[parser.current]; }
int current(void) { return parser.current; }

void report_error(ParseError *error) {
	printf("%s:%i:%i: ERROR Unexpected token; expected %s, got %s\n",
			parser.source.path,
			error->got.line,
			error->got.chr_index,
			token_to_str(error->expected),
			token_to_str(error->got.type)
			);
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

token_s peek(void) { return peek_n(1); }

token_s peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

token_s prev(void) { return prev_n(1); }

token_s prev_n(int offset)
{
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->token_list[parser.current - offset];
}

int check_next (token_type expected) { 
	// printf("next is: %s\n", token_to_str(peek().type));
	return check_next_n(1, expected);
}

int check_next_n(int offset, token_type expected) { 
	// printf("T: %i\n", peek_n(offset).type == expected);
	return (peek_n(offset).type == expected); 
}

int match_range(int start, int end) 
{
	if (peek().type >= start && peek().type <= end) {
		consume();
		return 1;
	}
	return 0;
}

int match(token_type expected) {
	if (check_next(expected)) {
		consume();
		return 1;
	}
	return 0;
}

int match_expect(token_type expected)
{
	if (check_next(expected)) {
		consume();
		return 1;
	}
	printf(
			"ERROR: Unexpected token %s at (%i:%i), expected %s\n"
			, token_to_str(peek().type)
			, current_tok().line
			, current_tok().chr_index
			, token_to_str(expected)
			);
	return 0;
}

void error_unexpected( token_s got, token_type expected ) {
	if (!parser.had_error) {
		parser.had_error = 1;
		parser.errors = vec_of(struct ParseError*, 8);
	}
	struct ParseError *err = malloc(sizeof(struct ParseError));
	err->got = got, err->expected = expected;
	vec_push(parser.errors, (struct ParseError*)err);
}

token_s token_at(long id)
{
	if (id >= parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[id];
}

literal_s lit_at(long tok_id)
{
	if (tok_id >= parser.tokens->current_token) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return (literal_s){.type=LIT_NONE};
	}
	int lit_id = token_at(tok_id).literal_id;	
	return parser.tokens->literal_list[lit_id];
}

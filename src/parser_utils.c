
#include "parser.h"
#include "scanner.h"
#include "parser_utils.h"

struct parser {
	token_array_s *tokens;
	int current;
};

static struct parser parser;

void init_parser(token_array_s *tokens) {
	parser.tokens = tokens;
	parser.current = -1;
}

token_s current_tok(void) { return parser.tokens->token_list[parser.current]; }
int current(void) { return parser.current; }


void consume(void) { parser.current++; }

token_s peek(void) { return peek_n(1); }

token_s peek_n(int offset) {
	if (parser.current + offset >= parser.tokens->current_token ) {
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

int expect(token_type expected) { return expect_n(1, expected); }

int expect_n(int offset, token_type expected) { return (peek_n(offset).type == expected); }

int match_range(int start, int end) 
{
	if (peek().type >= start && peek().type <= end) {
		consume();
		return 1;
	}
	return 0;
}

int match(token_type expected) {
	if (expect(expected)) {
		consume();
		return 1;
	}
	return 0;
}

int match_expect(token_type expected)
{
	if (expect(expected)) {
		consume();
		return 1;
	}
	error_unexpected(expected);
	return 0;
}

void error_unexpected( token_type expected ) {
	printf(
			"ERROR: Unexpected token %s at (%i:%i), expected %s\n"
			, token_to_str(peek().type)
			, current_tok().line
			, current_tok().chr_index
			, token_to_str(expected)
			);
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

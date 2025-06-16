/** @file internal data structures and functions for the parser subsystem */
#pragma once

#include "parser.h"

struct Parser {
	size current;
	bool had_error;
	dstr *source; // current file being parsed
	dbuf_ParseErrors *errors;
	dbuf_token *tokens;
	dbuf_Node * node_tree;
};

enum PanicType {
	P_CAT = 0,
	P_TOK = 1
};

// spooky global parser struct
static struct Parser parser;

/** returns the current token array index */
size current(void);
Token current_tok(void);

bool check_errors(void);

Token peek(void);
Token peek_n(int offset);

Token prev(void);
Token prev_n(int offset);
Token *token_at(size id);

dstr* cur_tok_span(void);

// prints an "Unexpected token error"

void error_unexpected( int got, int expected, bool is_tok, const char * file, const int line );
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(int tok);
int check_next_cat(int cat);
int check_next_n(int offset, int expected);
// match: if next token is as expected, consumes the token and returns true
int match(TokenTag type);
int match_range(int start, int end);
// int match_type_token(void);

void init_parser(dstr *src, dbuf_token* tokens);

void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line);


/** @file internal data structures and functions for the parser subsystem */
#pragma once

#include "parser.h"

struct Parser {
	bool had_error;
	dstr source; // current file being parsed
	dbuf_ParseErrors errors;
	dbuf_Node * node_tree;
	Token cur, prev;
};

enum PanicType {
	P_CAT = 0,
	P_TOK = 1
};

// spooky global parser struct
static struct Parser parser;

/** returns the current token array index */
Token cur_tok(void);
Token prev_tok(void);

bool check_errors(void);


// prints an "Unexpected token error"
void error_unexpected( Token got, int expected, bool is_tok, const char * file, const int line );
// get next token
Token consume(void);
// match: if next token is as expected, consumes the token and returns true
bool match(TokenTag tag);
Token expect(int tag, const char* msg);

void init_parser(dstr src);

void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line);


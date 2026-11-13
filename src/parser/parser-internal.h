/** @file internal data structures and functions for the parser subsystem */
#pragma once

#include "parser.h"

struct Parser {
	dstr file_name;

	bool had_error;
	bool panic_mode;

	dstr source; // current file being parsed
	dbuf_ParseErrors errors;
	// dbuf_Node * node_tree;
	Token cur, prev;
	d_arena arena;
};

enum PanicType {
	P_CAT = 0,
	P_TOK = 1
};

// spooky global parser struct
extern struct Parser parser;

/** returns the current scanned token */
Token cur_tok(void);
/** returns the previous scanned token */
Token prev_tok(void);
// match: if next token is as expected, consumes the token and returns true
bool match(TokenTag tag);
// consumes current token if it matches tag, else throws an unexpected token error
Token expect(int tag, const char* msg);

void report_errors(void);

const char* node_to_str(int node_tag);

// prints an "Unexpected token error"
void error_expected( int expected, bool is_tok, const char* msg );
void error_at(Token at, const char* msg);
void error(const char* msg);

// get next token
Token consume(void);

void synchronize(void);

void init_parser(dstr src, dstr file_name, d_arena arena);

void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line);


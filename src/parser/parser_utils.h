#pragma once

#include "../scanner.h"

void init_parser(struct File* source, struct TokenArray* tokens);

int current(void);
struct Token *current_tok(void);
struct Literal *cur_lit();
struct Literal *lit_at(long tok_id);

int check_errors();

struct Token peek(void);
struct Token peek_n(int offset);

struct Token prev(void);
struct Token prev_n(int offset);

struct Token *token_at(long id);

// prints an "Unexpected token error"
void error_unexpected_cat( struct Token *got, int expected, const char * file, const int line );
void error_unexpected_tok( struct Token *got, int expected, const char * file, const int line );
void error_unexpected( int got, int expected, int is_tok, const char * file, const int line );
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(int tok);
int check_type_token(void);
int check_next_cat(int cat);
int check_next_n(int offset, int expected);
// match: if next token is as expected, consumes the token and returns true
int match(int type);
int match_cat(int cat);
int match_range(int start, int end);
int match_type_token(void);

// prints an "Unexpected token error" if the next token isn't as expected
// int match_or_err(token_type expected);

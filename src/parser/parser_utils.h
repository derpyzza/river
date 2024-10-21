#pragma once

#include "../scanner.h"
#include "../common.h"

void init_parser(file_s source, token_array_s* tokens);


int current(void);
struct token_s current_tok(void);

int check_errors();


struct token_s peek(void);
struct token_s peek_n(int offset);

struct token_s prev(void);
struct token_s prev_n(int offset);

struct literal_s *lit_at(long tok_id);
struct literal_s *cur_lit();
struct token_s token_at(long id);

// prints an "Unexpected token error"
void error_unexpected_cat( token_s got, tokcat expected, const char * file, const int line );
void error_unexpected_tok( token_s got, token_type expected, const char * file, const int line );
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(token_type tok);
int check_next_cat(tokcat cat);
int check_next_n(int offset, token_type expected);
// match: if next token is as expected, consumes the token and returns true
int match(token_type type);
int match_range(int start, int end);
int match_type_token();

// prints an "Unexpected token error" if the next token isn't as expected
// int match_or_err(token_type expected);

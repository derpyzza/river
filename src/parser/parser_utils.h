#pragma once

#include "../scanner.h"

void init_parser(struct token_array_s* tokens);


int current(void);
struct token_s current_tok(void);


struct token_s peek(void);
struct token_s peek_n(int offset);

struct token_s prev(void);
struct token_s prev_n(int offset);

struct literal_s lit_at(long tok_id);
struct token_s token_at(long id);

// prints an "Unexpected token error"
// void error_unexpected(struct node* node, token_type expected);
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(token_type type);
int check_next_n(int offset, token_type expected);
// match: if next token is as expected, consumes the token and returns true
int match(token_type type);
int match_range(int start, int end);
// prints an "Unexpected token error" if the next token isn't as expected
int match_expect(token_type expected);

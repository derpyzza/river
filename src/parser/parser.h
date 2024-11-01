#pragma once

#include "../scanner.h"

typedef struct ParseError {
	int is_tok;
	int type;
	int debug_line;
	int got;
	// char *msg;
	const char * debug_file;
} ParseError;

// typedef struct Var {
// 	int is_assign;
// 	struct Substr *name;
// 	struct Literal *val;
// } Var;

// this can be used for import paths
// but also for namespacing!
// i.e import x.y.z.w;
// or X::Y::Z::W.do_something();

typedef struct Path {
	int has_subpaths;
	int cur_subpath;
	struct String full;
	struct String root;
	struct String subpath[32];
} Path;

enum NodeTag {
	N_NONE,
	N_NODE_LIST, // hack
	N_PROG,
	N_EXPR,
	N_BIN_EXPR,
	N_UN_EXPR,
	N_LIT,
	N_BEXPR,
	N_UNEXPR,
	N_STMT,
	N_BLOCK,
	N_FUNC_DEF,
	N_IMPORT,
	N_STRUCT_DEF,
	N_STRUCT_FIELD,
	N_IF,
	N_WHILE,
	N_FOR,
	N_FOREACH,
	N_VAR,
	N_CONST
};

// ineffeciently laid out struct but meh.
// can change this later if it bottlenecks somehow
struct Node {
	// static type hint
	int type;
	// is an identifier
	int has_name;
	// index of name substring in literals array
	int name_id; 
	// self explanatory
	int is_stmt;
	/** 
	* index of value in the literals array; 
	* literally just the same as name_id but a node
	* could have both an identifier and also a value
	* which is why there's two of them
	*/ 
	int value;

	enum NodeTag tag;
	struct Vec* children;
	// for import or method paths
	struct Path path;

	// for expressions
	struct Node *lhs, *rhs;

	// handy for functions and control flow expressions.
	struct Node
		*cond, 	// condition
		*els, 	// else-clause
		*body, 	// body
		*init,	// initial expression, for `for` loops
		*inc;		// increment expression, for `for` loops
};

struct Node *parse_tokens( struct TokenArray *tokens, struct File *src ); 
struct Node *new_node(enum NodeTag tag);
void print_ast(struct Node node);
struct ParseError parse_error(int expected);

int current(void);
struct Token current_tok(void);

int check_errors();


struct Token peek(void);
struct Token peek_n(int offset);

struct Token prev(void);
struct Token prev_n(int offset);

void print_lit(struct Literal* lit);

struct Literal *lit_id(long tok_id);
struct Literal *lit_at_tok(long tok_id);
struct Literal *cur_lit();
struct Token *token_at(long id);

// prints an "Unexpected token error"

void error_unexpected( int got, int expected, int is_tok, const char * file, const int line );
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(int tok);
int check_next_cat(int cat);
int check_next_n(int offset, int expected);
// match: if next token is as expected, consumes the token and returns true
int match(TokenType type);
int match_range(int start, int end);
int match_type_token();

// prints an "Unexpected token error" if the next token isn't as expected
// int match_or_err(token_type expected);

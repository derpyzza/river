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
	int type;
	int has_name;
	int name_id; // index of name substring in literals array
	int is_stmt;

	enum NodeTag tag;
	struct Vec* children;

	// for expressions
	struct Node *lhs, *rhs;
	struct Path path;
	
	struct Literal *value;

	// for functions and control flow expressions.
	struct Node
		*cond,
		*els,
		*body,
		*init,
		*inc;
};


struct Parser {
	int current;
	struct File *source; // current file being parsed
	struct TokenArray *tokens;
	int had_error;
	struct Vec *errors;
};

// spooky global parser struct
static struct Parser parser;

struct Node *parse_tokens( struct TokenArray *tokens, struct File *src ); 
struct Node *new_node(enum NodeTag tag);
void print_ast(struct Node node);
struct ParseError parse_error(int expected);

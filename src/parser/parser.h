#pragma once

#include "../scanner.h"
#include "expr.h"

typedef struct ParseError {
	int is_tok;
	union {
		token_type tok;
		tokcat cat;
	};
	// char *msg;
	token_s got;
	const char * debug_file;
	int debug_line;
} ParseError;

typedef struct Var {
	substr_s name;
	int is_assign;
	literal_s val;
} Var;

typedef struct AssignedVar {
	token_type name;
	// you can declare 16 variables in a comma separated list.
	// after that you'll have to switch to a newline;
	// i.e 
	// int q, w, e, r, t, y, u, i, o, p, a, s, d, f, g, h; // <- stop at 'h'
	// int j, k, l; // ... and so on.
	//
	// this is because parsing is a pain and only an insane person
	// would declare more than 16 variables in one line.
	struct Var val[16];
} AssignedVar;

// this can be used for import paths
// but also for namespacing!
// i.e import x.y.z.w;
// or X::Y::Z::W.do_something();
typedef struct Path {
	substr_s full;
	substr_s root;
	int has_subpaths;
	int cur_subpath;
	substr_s subpath[32];
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
	enum NodeTag tag;
	struct Vec* children;

	// for expressions
	struct Node *lhs, *rhs;
	struct Path path;

	token_type type;
	int has_name;
	int is_stmt;
	substr_s name;
	
	long int value;

	// for functions and control flow expressions.
	struct Node
		*cond,
		*els,
		*body,
		*init,
		*inc;
};

struct Node *parse_tokens( token_array_s *tokens, file_s src ); 
struct Node *new_node(enum NodeTag tag);
void print_ast(struct Node node);
struct ParseError parse_error(token_type expected);
// char* node_to_string(char* string, node_s node);

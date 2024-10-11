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
typedef struct path {
	substr_s full;
	substr_s root;
	int has_subpaths;
	int cur_subpath;
	substr_s subpath[32];
} path;

enum node_tag {
	N_NONE,
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
	N_IF,
	N_WHILE,
	N_FOR,
	N_FOREACH,
};

struct node {
	enum node_tag tag;
	struct Vec* children;

	// for expressions
	struct node *lhs, *rhs;
	struct path path;

	int type;
	int has_name;
	substr_s name;
	
	long long int value;

	// for loops and if conditions
	struct node
		*cond,
		*els,
		*body,
		*init,
		*inc;
};

struct node *parse_tokens( token_array_s *tokens, file_s src ); 
struct node *new_node(enum node_tag tag);
void print_ast(struct node node);
struct ParseError parse_error(token_type expected);
// char* node_to_string(char* string, node_s node);

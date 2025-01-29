#pragma once

#include <stdint.h>
#include <stdio.h>
#include "../scanner.h"

typedef struct ParseError {
	bool is_tok; // guess
	int type; // token / cat type
	int debug_line; // compiler source line that the error is emitted from
	int got; // index to got token
	// char *msg;
	const char * debug_file;
} ParseError;

CREATE_VEC_TYPE(ParseError, ParseErrors)

typedef struct Datatype Datatype;
typedef union DatatypeUnion DatatypeUnion;
typedef enum DatatypeTag DatatypeTag;

CREATE_VEC_TYPE(Datatype*, Datatype);

enum DatatypeTag {
	DATATYPE_IDENTIFIER,
	DATATYPE_POINTER,
	DATATYPE_ARRAY,
	DATATYPE_TUPLE,
};

union DatatypeUnion {
	String * dt_iden;
	struct Pointer {
		Datatype * to;
	} *dt_ptr;
	struct Array {
		Datatype * of;
		size size;
	} * dt_arr;
	VecDatatype* dt_tuple;
};

struct Datatype {
	DatatypeTag tag;	
	DatatypeUnion* type;
};
	

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
	bool has_subpaths;
	int cur_subpath;
	String full;
	String root;
	String subpath[32];
} Path;

typedef enum NodeTag {
	N_NONE,
	N_TODO,
	N_NODE_LIST, // hack
	N_PROG,
	N_EXPR,
	N_BIN_EXPR,
	N_UN_EXPR,
	N_LIT,
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
} NodeTag;

// NOTE: 
// ineffeciently laid out struct but meh.
// can change this later if it bottlenecks somehow

// master node struct that represents a single node of the AST.
// just haphazardly stuffed with every possible value a node could need.

// most data is stored as just an index to an item in the token stream's literal array, 
// rather than storing pointers or copies of the required values.
// I'm storing the literal arrays anyway, might as well squeeze as much use out of them as possible!
typedef struct Node Node;
struct Node {
	TokenTag op;
	String *type; 	// static type hint
	String *name; 	// name bound to this object
	String *value; 	// literal value

	NodeTag tag;
	struct VecNode* children;
	// for import or method paths
	Path path;

	// for expressions
	Node *lhs, *rhs;

	// handy for functions and control flow expressions.
	Node
		*cond, 	// condition
		*els, 	// else-clause
		*body, 	// body
		*init,	// initial expression, for `for` loops
		*inc;		// increment expression, for `for` loops
};

CREATE_VEC_TYPE(Node*, Node)

Node *parse_tokens( VecToken *tokens, File *src ); 
Node *new_node( NodeTag tag );
void print_ast( Node *node, int level );
ParseError parse_error(int expected);

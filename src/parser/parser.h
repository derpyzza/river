#pragma once

#include <stdint.h>
#include <stdio.h>
// #include "../memory.h"
#include "../scanner.h"

typedef struct ParseError {
	bool is_tok; // guess
	int type; // token / cat type
	int debug_line; // compiler source line that the error is emitted from
	int got; // index to got token
	// char *msg;
	const char * debug_file;
} ParseError;

dbuf_decl(ParseError, ParseErrors)

typedef struct Datatype Datatype;
typedef union DatatypeUnion DatatypeUnion;

dbuf_decl(Datatype*, Datatype)

enum BasicDataTypeTag {
	BDT_INT,
	BDT_UINT,
	BDT_UINT8,
	BDT_UINT16,
	BDT_UINT32,
	BDT_UINT64,
	BDT_INT8,
	BDT_INT16,
	BDT_INT32,
	BDT_INT64,
	BDT_FLOAT32,
	BDT_FLOAT64,
	BDT_BOOL,
	BDT_STRING,
	BDT_SIZE,
	BDT_USIZE,
	BDT_VOID,
	BDT_ANYPTR,
};

typedef enum DatatypeTag {
	DATATYPE_IDENTIFIER,
	DATATYPE_POINTER,
	DATATYPE_ARRAY,
	DATATYPE_SLICE,
	DATATYPE_FUNC,
	DATATYPE_TUPLE,
} DatatypeTag;

#define MAX_PTR_DEPTH 8

union DatatypeUnion {
	dstr dt_iden;
	struct Pointer {
		Datatype * to;
	} dt_ptr;
	struct Array {
		int size;
		Datatype * of;
	} dt_arr;
	struct Func {
		Datatype * return_type;
		struct Params {
			dstr iden;
			dstr def_value;
			Datatype * type;
		} * params;
	} func;
	dbuf_Datatype dt_tuple;
};

struct Datatype {
	DatatypeTag tag;	
	DatatypeUnion* type;
};
	
// this can be used for import paths
// but also for namespacing!
// i.e import x.y.z.w;
// or X::Y::Z::W.do_something();

typedef struct Path {
	bool has_subpaths;
	int cur_subpath;
	dstr full;
	dstr root;
	dstr subpath[32];
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
typedef struct Node Node;
dbuf_decl(Node**, NodeList) // array of pointers to nodes
struct Node {
	TokenTag op;
	Datatype * type; // optional
	dstr iden; 	// name bound to this object
	dstr value; // literal value

	NodeTag tag;
	dbuf_NodeList* children;
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
dbuf_decl(Node, Node)


dbuf_Node *parse_tokens( dbuf_token *tokens, dstr src ); 
Node* new_node( NodeTag tag );
void print_ast( Node *node, int level );
ParseError parse_error(int expected);

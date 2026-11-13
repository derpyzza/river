#pragma once

#include <stdint.h>
#include <stdio.h>
// #include "../memory.h"
#include "../scanner.h"

typedef struct Node          Node;
typedef struct Param         Param;
typedef struct Datatype      Datatype;
typedef struct ParseError    ParseError;
typedef union  DatatypeUnion DatatypeUnion;

typedef enum {
    PERR_EXPECT_TOK,
    PERR_EXPECT_CAT,
    PERR_LEX,          // scanner returned T_ERROR
    PERR_MSG,          // free-form message only
} ParseErrorKind;

struct ParseError {
	ParseErrorKind kind;
	int expected;   // token / cat type
	Token got;
	// int got_id;     // index to got token
	const char * msg;

# ifdef DEBUG
	// [debug] these are only for debug
	int debug_line;          // compiler source line that the error is emitted from
	const char * debug_file; // compiler source file the error is emitted from
# endif
};

dbuf_decl(ParseError, ParseErrors)

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

// used by function prototypes, structs, and unions;
struct Param {
	dstr iden;
	Node * def_value;
	Datatype * type;
};

dbuf_decl(Param, Param)

union DatatypeUnion {
	dstr dt_iden;

	struct Pointer {
		// level of pointer indirection ( i.e int* vs int** etc. ).
		// default is 1 level of indirection.
		int indir;
		Datatype * to;
	} dt_ptr;

	struct Array {
		int size;
		Datatype * of;
	} dt_arr;

	struct Slice {
		int size;
		Datatype * of;
	} dt_slice;

	struct Func {
		Datatype * return_type;
		dbuf_Param params;
	} dt_func;

	dbuf_Datatype dt_tuple;
};

struct Datatype {
	DatatypeTag tag;	
	DatatypeUnion type;
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

#define NODES                             \
  NODE(N_NONE,         "none")            \
  NODE(N_TODO,         "todo")            \
  NODE(N_NODE_LIST,    "node_list")       \
  NODE(N_PROG,         "prog")            \
  NODE(N_EXPR_BLOCK,   "expr_block")      \
  NODE(N_EXPR,         "expr")            \
  NODE(N_LIT,          "lit")             \
  NODE(N_STMT,         "stmt")            \
  NODE(N_BLOCK,        "block")           \
  NODE(N_FUNC_DEF,     "func_def")        \
  NODE(N_IMPORT,       "import")          \
  NODE(N_STRUCT_DEF,   "struct_def")      \
  NODE(N_STRUCT_FIELD, "struct_field")    \
  NODE(N_IF,           "if")              \
  NODE(N_WHILE,        "while")           \
  NODE(N_FOR,          "for")             \
  NODE(N_FOREACH,      "foreach")         \
  NODE(N_VAR,          "var")             \
  NODE(N_CONST,        "const")           \

typedef enum NodeTag {
	#define NODE(N, S) N,
	NODES
	#undef NODE
} NodeTag;

dbuf_decl(Node *, NodeList)

struct Node {
	NodeTag tag;
	int line;
	dstr iden;
	dstr value;
	int op;

	Path path;

	Datatype type;
	Node * lhs, * rhs;
	Node * cond, * body, *els; // if Cond, Body, Els
	Node * init, * inc; // for Init; Cond; Inc
	dbuf_NodeList children;
};

dbuf_decl(Node, Node)


Node* parse_tokens(dstr source, dstr file_name, d_arena arena);
Node* new_node( NodeTag tag );
void print_ast( Node *node, int level );

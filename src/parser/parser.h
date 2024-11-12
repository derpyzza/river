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

CREATE_VEC_TYPE(ParseError, ParseErrors);

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
} NodeTag;

typedef enum RT_Type {
	RT_ISIZE,
	RT_USIZE,
	
	RT_BYTE,
	RT_SHORT,
	RT_INT,
	RT_LONG,

	RT_UBYTE,
	RT_USHORT,
	RT_UINT,
	RT_ULONG,

	RT_FLOAT,
	RT_DOUBLE,

	RT_CHAR,
	RT_STRING,
	RT_BOOL,
	RT_VOID, // only for function return types
	RT_BUILT_IN_TYPES, // handy for iteration

	RT_STRUCT,
	RT_TUPLE,
	RT_ENUM,
	RT_UNION,
	RT_ALIAS, // typedef
	RT_POINTER,
	RT_FUNCTION,
	RT_ARRAY,
} RT_Type;

typedef union RT_Union RT_Union;
union RT_Union {

	// pointer sized integers — the return type of the sizeof operator
	size isize_t; 	// signed
	usize usize_t;	// unsigned
	
	// unsigned integer types
	uint64_t uinteger_t;
	// signed integer types
	int64_t integer_t;

	// floating point types
	double floating_t;				
	long double long_floating_t; // in case i need a longer float

	bool bool_t; // boolean type. 
	uint char_t; // char in river. single utf-8 character

	struct {
		int indirection; // levels of indirection; upto 8;
		RT_Union *ptr;
	}pointer_t;

	struct {
		size len;
		RT_Union *type;
	} array_t;

	struct {
		uint *start;
		size len;
	} string_t;

	struct {
		int offset;
		Vec members;
	} struct_t;
	// struct {} tuple_t;
	// struct {} union_t;
	// struct {} enum_t;
	struct {
		RT_Union *returnType;
		String name;
	} function_t;

	struct {
		RT_Union *type;
		String alias;
	} typealias;
	
};

typedef struct RType {
	RT_Type tag;
	RT_Union value;
} RType;

// NOTE: 
// ineffeciently laid out struct but meh.
// can change this later if it bottlenecks somehow

// master node struct that represents a single node of the AST.
// just haphazardly stuffed with every possible value a node could need.

// most data is stored as just an index to an item in the token stream's literal array, 
// rather than storing pointers or copies of the required values.
// I'm storing the literal arrays anyway, might as well squeeze as much use out of them as possible!
typedef struct VecNode VecNode;
typedef struct Node Node;
struct Node {
	TokenType op;
	String *type; 	// static type hint
	String *name; 	// name bound to this object
	String *value; 	// literal value

	NodeTag tag;
	VecNode* children;
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

CREATE_VEC_TYPE(Node*, Node);

Node *parse_tokens( VecToken *tokens, File *src ); 
Node *new_node( NodeTag tag );
void print_ast( Node *node, int level );
ParseError parse_error(int expected);

/** returns the current token array index */
size current(void);
Token current_tok(void);

bool check_errors(void);

Token peek(void);
Token peek_n(int offset);

Token prev(void);
Token prev_n(int offset);
Token *token_at(size id);

String* cur_tok_span(void);

// prints an "Unexpected token error"

void error_unexpected( int got, int expected, bool is_tok, const char * file, const int line );
void consume(void);
// expect: returns whether or not the next token is as expected
int check_next(int tok);
int check_next_cat(int cat);
int check_next_n(int offset, int expected);
// match: if next token is as expected, consumes the token and returns true
int match(TokenType type);
int match_range(int start, int end);
int match_type_token(void);


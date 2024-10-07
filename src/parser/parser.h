#pragma once

#include "../scanner.h"
#include <stdio.h>
typedef enum {
	N_ERROR = -1,
	N_NONE = 0,
	N_EXP_UN,
	N_EXP_BIN,

	// literals:
	N_LIT_INT,
	N_LIT_DOUBLE,
	N_LIT_TRUE,
	N_LIT_FALSE,
	N_LIT_NULL,
	N_LIT_STRING,

	N_BLOCK,

	N_FN_CALL,
	N_FN_DEF,
	N_VAR_ASSIGN,

	N_STMT,
	N_RETURN,
} node_type;

typedef struct ParseError {
	token_type expected;
	// char *msg;
	token_s got;
} ParseError;

// typedef literal_s NodeLit;

typedef struct NodeExprStmt {} NodeExprStmt ;
typedef struct NodeVarStmt {} NodeVarStmt ;
typedef struct NodeStmt {
	enum StmtType {
		S_UNKNOWN,
		S_VAR,
		S_EXPR,
	} type;
} NodeStmt;

typedef struct NodeBlock {
	struct Vec *statements;
	int has_ret_expr;
	struct NodeExpr* ret_expr;
} NodeBlock;

typedef struct NodeUnionDef {} NodeUnionDef;
typedef struct NodeEnumDef {} NodeEnumDef;
typedef struct NodeTypeDef {} NodeTypeDef;
typedef struct NodeStructDef {} NodeStructDef;

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

// TYPE IDEN (PARAMS)* => ( expr ";" | BlockExpr )
typedef struct NodeFuncDef {
	int had_error;

	union {
		ParseError err;
		struct {
			substr_s name;
			token_type return_type;
			// river only allows 64 parameters given into a function.
			// womp womp.
			struct AssignedVar params[64]; 
			union {
				struct NodeExpr *expr;
				struct NodeBlockExpr *body;
			};
		};
	};
} NodeFuncDef;

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

// "import" IDENTIFIER(.IDENTIFIER)*? (as IDENTIFIER)? ";"
typedef struct NodeImportDef {

	int had_error;
	union {
		ParseError err;
		struct {
			struct Path path;
			int has_alias;
			substr_s alias;
		};
	};

} NodeImportDef;


// Global variable definitions:
// MUST:
// 	have an explicit type attached ( i.e not 'val' )
// 	be initialized with a value
// CAN OPTIONALLY:
//	have the static, const, or mut modifiers before the type.
//	the mut modifier cannot be combined with the const modifier,
//	but all other combinations of the three are valid ( i.e const + static, static + mut )
typedef struct NodeGlobalDef {
	token_type type;
	int num_vars;
	substr_s* lhs;
	struct NodeExpr* rhs;
} NodeGlobalDef;

typedef struct NodeItem {
	enum ItemType{
		I_UNKNOWN,
		I_FN_DEF,
		I_IMPORT_DEF,
		I_GLOBAL_DEF,
		I_STRUCT_DEF,
		I_ENUM_DEF,
		I_TYPE_DEF,
		I_UNION_DEF
	}type;

	union {
		struct NodeGlobalDef *global_def;
		struct NodeImportDef *imp_def;
		struct NodeFuncDef *fn_def;
		struct NodeStructDef *struct_def;
		struct NodeTypeDef *type_def;
		struct NodeEnumDef *enum_def;
		struct NodeUnionDef *union_def;
	};
} NodeItem;

typedef struct NodeProg {
	struct Vec* children;
} NodeProg;


// Generic tree node that holds references to all node types in a union,
// as a parent type.
// this way i can have functions that operate on all node types rather than having
// typedef struct Node {
//
// } Node;


// modifier flags:
// 0001 0000 = public
// 0000 0001 = static
// 0000 0010 = mutable
// 0000 0100 = const

void print_ast(NodeProg node);
struct NodeProg *parse_tokens( struct token_array_s *tokens, struct string_s src ); 
struct ParseError parse_error(token_type expected);
// char* node_to_string(char* string, node_s node);

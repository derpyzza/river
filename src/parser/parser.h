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

union expr{
	struct binary {
		struct node *lhs, *rhs;
		int operator_type;
	} binary;
	struct unary {
		struct node *operand;
		int operator_type;
	} unary;
};

struct var {
	token_type type;
	substr_s id;
};

struct func_sig {
	substr_s func_name;
	token_type return_type;
	int num_params;
	struct var* params;
};

union Stmt {
	union expr expr;
};

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

typedef struct Val {
	substr_s name;
	token_type type;
	int is_assign;
	literal_s val;
} Val;

// TYPE IDEN (PARAMS)* => ( expr ";" | BlockExpr )
typedef struct NodeFuncDef {
	substr_s name;
	token_type return_type;
	struct Vec *params; // Vec<NodeFnParam>
	union {
		struct NodeExpr *expr;
		struct NodeBlock *block;
	} body;
} NodeFuncDef;

// "import" IDENTIFIER(.IDENTIFIER)*? (as IDENTIFIER)? ";"
typedef struct NodeImportDef {} NodeImportDef;


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
		I_UNION_DEF,
		I_STMT
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
	int num_children;
	struct NodeItem** children;
} NodeProg;

// modifier flags:
// 0001 0000 = public
// 0000 0001 = static
// 0000 0010 = mutable
// 0000 0100 = const

// AST Node 

// void print_ast(NodeProg node);
struct NodeProg *parse_tokens( struct token_array_s *tokens, struct string_s src ); 
// char* node_to_string(char* string, node_s node);

#include <math.h>
#include <stdio.h>

#include "../scanner.h"
#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

// Forward declarations so that i can rearrange the definitions as i wish
// static char* node_to_str(node_s node);

// static node_s *new_node(node_type type);
// static node_s *node_error(token_type expected);
// static node_s *new_bin_expr(node_s *lhs, node_s *rhs, int op);
// static node_s *new_unary_expr(node_s *right, int op);

static struct NodeProg *program(void);
static struct NodeItem *n_item(void);
static struct NodeGlobalDef *global_def(void);
static struct NodeImportDef *imp_def(void);
static struct NodeFuncDef *fn_def(void);
static struct NodeStructDef *struct_def(void);
static struct NodeTypeDef *type_def(void);
static struct NodeEnumDef *enum_def(void);
static struct NodeUnionDef *union_def(void);
static struct NodeBlock *block(void);
static struct NodeStmt *stmt(void);
static struct NodeVarStmt *var_stmt(void);
static struct NodeExprStmt *expr_stmt(void);

struct NodeProg *parse_tokens( token_array_s *tokens, string_s src ) 
{ 
	init_parser(tokens);

	struct NodeProg *node = program();

	return node;
}

// static node_s *parser_panic(node_s* node, token_type expected, token_type until) {
// 	error_unexpected(node, expected);
// 	while(!check_next(T_EOF) && !check_next(until)) consume();
// 	node->type = N_NONE;
// 	return node;
// }

static struct NodeProg *program(void) {


	NodeProg* node = malloc(sizeof(struct NodeProg));
	node->num_children = 0;
	// just allocates 8 items to start out with.
	// should move all allocations to a big buffer later, rather than individual 
	// tiny allocations like here:
	node->children = (NodeItem**) malloc(sizeof(NodeItem*) * 8);

	while(!check_next(T_EOF)) {
		NodeItem* item = n_item();
		if(item->type != I_UNKNOWN) {
			node->children[node->num_children] = item;
			node->num_children++;
		}
	}
}

static struct NodeItem *n_item(void) {
	NodeItem* item = malloc(sizeof(struct NodeProg));


	if (match(T_STATIC) || match(T_CONST)) {
		if (match(T_IDENTIFIER) || match_range(T_UBYTE, T_BOOL)) {
			token_type type = current_tok().type;
			printf("Matched type %s\n", token_to_str(type));
			if (match(T_IDENTIFIER)) {
				substr_s name = lit_at(current()).literal._str;
				if (match(T_COMMA) || match(T_SEMI) || match(T_EQUAL_SIGN)) {
					item->type = I_GLOBAL_DEF;
					item->global_def = global_def();
					return item;
				}
				if (match(T_PAREN_OPEN) || match(T_FAT_ARROW)) {
					item->type = I_FN_DEF;
					item->fn_def = fn_def();
					item->fn_def->name = name;
					item->fn_def->return_type = type;
					return item;
				}
			}
		}
	}

	return NULL;
	// if (match(T_STRUCT)) {
	// 	// struct def
	// }
	// if (match(T_TYPE)) {
	// 	// type def
	// }
	// if (match(T_ENUM)) {
	// 	// enum def
	// }
	// if (match(T_IMPORT)) {
	// 	// imp def
	// }
}

static struct NodeFuncDef *fn_def(void) {
	// parse params
	if(current_tok().type == T_PAREN_OPEN) {
		while(!match(T_PAREN_CLOSE)) {

		}
	}

	// parse block
	if(match(T_BRACE_OPEN)) {
		while(!match(T_BRACE_CLOSE)) {

		}
	}
	return NULL;
}


static struct NodeGlobalDef *global_def(void) {
	return NULL;
}

// void print_ast(node_s node)
// {
//
// }

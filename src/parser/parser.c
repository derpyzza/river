#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../scanner.h"
#include "../utils.h"
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
	// just allocates 8 items to start out with.
	// should move all allocations to a big buffer later, rather than individual 
	// tiny allocations like here:
	node->children = new_vec();

	while(!check_next(T_EOF)) {
		NodeItem* item = n_item();
		if(item->type != I_UNKNOWN) {
			vec_push(node->children, item);
		}
	}
	return node;
}

static struct NodeItem *n_item(void) {
	NodeItem* item = malloc(sizeof(struct NodeItem));
	item->type = I_UNKNOWN;

	if (match(T_IMPORT)) {
		item->type = I_IMPORT_DEF;
		item->imp_def = imp_def();
	}

	// if (match(T_STATIC) || match(T_CONST)) {
	// 	if (match(T_IDENTIFIER) || match_range(T_UBYTE, T_BOOL)) {
	// 		token_type type = current_tok().type;
	// 		printf("Matched type %s\n", token_to_str(type));
	// 		if (match(T_IDENTIFIER)) {
	// 			substr_s name = lit_at(current()).literal._str;
	// 			if (match(T_COMMA) || match(T_SEMI) || match(T_EQUAL_SIGN)) {
	// 				item->type = I_GLOBAL_DEF;
	// 				item->global_def = global_def();
	// 				return item;
	// 			}
	// 			if (match(T_PAREN_OPEN) || match(T_FAT_ARROW)) {
	// 				item->type = I_FN_DEF;
	// 				item->fn_def = fn_def();
	// 				item->fn_def->name = name;
	// 				item->fn_def->return_type = type;
	// 				return item;
	// 			}
	// 		}
	// 	}
	// }

	return item;
	// if (match(T_STRUCT)) {
	// 	// struct def
	// }
	// if (match(T_TYPE)) {
	// 	// type def
	// }
	// if (match(T_ENUM)) {
	// 	// enum def
	// }
}

static struct NodeImportDef *imp_def(void) {
	NodeImportDef *imp = malloc(sizeof(NodeImportDef));
	imp->had_error = 0;

	if (match(T_IDENTIFIER)) {
		imp->path.root = lit_at(current()).literal._str;
		imp->path.full = lit_at(current()).literal._str;
		if(check_next(T_DOT)) {
			imp->path.has_subpaths = 1;
			// imp->path.subpath[imp->path.cur_subpath];

			while(match(T_DOT)) {
				if(!match(T_IDENTIFIER)) {
					imp->had_error = 1;
					imp->err = parse_error(T_IDENTIFIER, current_tok());
					return imp;
				}
				imp->path.subpath[imp->path.cur_subpath] = lit_at(current()).literal._str; 
				imp->path.full.len += lit_at(current()).literal._str.len + 1;
				imp->path.cur_subpath++;
			}
			if(match(T_AS)) {
				if(!match(T_IDENTIFIER)) {
					imp->had_error = 1;
					imp->err = parse_error(T_IDENTIFIER, current_tok());
					return imp;
				}
				imp->has_alias = 1;
				imp->alias = lit_at(current()).literal._str;
			}
		}
		if (!match(T_SEMI)) {
			imp->had_error = 1;
			imp->err = parse_error(T_SEMI, current_tok());
			return imp;
		}
		return imp;
	} else {
		// error
		imp->had_error = 1;
		imp->err = parse_error(T_IDENTIFIER, current_tok());
		return imp;
	}
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

struct ParseError parse_error(token_type expected, token_s got) {
	return (ParseError) { .expected = expected, .got = got };
}

void print_ast(NodeProg node)
{
	for ( int i = 0; i < node.children->current; i++ ) {
		NodeItem *cur = node.children->data[i];
		switch(cur->type) {
			case I_UNKNOWN: printf("Unknown!\n"); break;
			case I_FN_DEF: printf("fn def\n"); break;
			case I_IMPORT_DEF: {
				substr_s root = cur->imp_def->path.root;
				printf("<imp def> path: %.*s, root: %.*s", 
						cur->imp_def->path.full.len,
						cur->imp_def->path.full.c_ptr,
						root.len, root.c_ptr
						);
				if (cur->imp_def->has_alias) 
					printf(", alias: %.*s",
						cur->imp_def->alias.len,
						cur->imp_def->alias.c_ptr
							);
				printf("\n");
				break;
			}
			case I_GLOBAL_DEF: printf("global def\n"); break;
			case I_STRUCT_DEF: printf("struct def\n"); break;
			case I_ENUM_DEF: printf("enum def\n"); break;
			case I_TYPE_DEF: printf("type def\n"); break;
			case I_UNION_DEF: printf("union def\n"); break;
		}
	}
}

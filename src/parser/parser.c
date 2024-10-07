#include <stdio.h>
#include <string.h>

#include "../scanner.h"
#include "../utils.h"
#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

// Forward declarations so that i can rearrange the definitions as i wish
static struct NodeProg *program(void);
static struct NodeItem *n_item(void);
static struct NodeGlobalDef *global_def(void);
static struct NodeImportDef *imp_def(void);
static struct NodeFuncDef *fn_def(void);
static struct NodeStructDef *struct_def(void);
static struct NodeTypeDef *type_def(void);
static struct NodeEnumDef *enum_def(void);
static struct NodeUnionDef *union_def(void);
static struct NodeStmt *stmt(void);
static struct NodeVarStmt *var_stmt(void);
static struct NodeExprStmt *expr_stmt(void);

struct NodeProg *parse_tokens( token_array_s *tokens, string_s src ) 
{ 
	init_parser(tokens);
	// print_token_array(src, *tokens);
	// for (int x = 0; x <= tokens->current_token + 4; x++) {
	// 	printf("[%i] actual: %s, peek: %s\n", 
	// 		x,
	// 		token_to_str(tokens->token_list[x].type),
	// 		token_to_str(peek_n(x).type));
	// }
	// printf("max tokens: %li\n", tokens->current_token);

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
	node->children = new_vec();

	while(!match(T_EOF)) {
		// printf("current: %i\n", current());
		// printf("stuck here!\n");
		NodeItem* item = n_item();
		if(item->type != I_UNKNOWN) {
			vec_push(node->children, item);
		}
		// print_ast(*node);
	}
	return node;
}

static struct NodeItem *n_item(void) {
	NodeItem* item = malloc(sizeof(struct NodeItem));
	item->type = I_UNKNOWN;

	if (match(T_IMPORT)) {
		item->imp_def = imp_def();
		item->type = I_IMPORT_DEF;
		return item;
	}

	// if (match(T_STRUCT)) {
	// 	item->type = I_STRUCT_DEF;
	// 	item->struct_def = struct_def();
	// 	return item;
	// }
	//
	// if (match(T_FN)) {
	// 	item->type = I_FN_DEF;
	// 	item->fn_def = fn_def();
	// 	return item;
	// }
	//
	// if (match(T_CONST)) {
	// 	item->type = I_GLOBAL_DEF;
	// 	item->global_def = global_def();
	// 	return item;
	// }	

	return item;
}

static struct NodeImportDef *imp_def(void) {
	NodeImportDef *imp = malloc(sizeof(NodeImportDef));
	imp->had_error = 0;

	if (!match(T_IDENTIFIER)) 
	{
		// error
		imp->had_error = 1;
		imp->err = parse_error(T_IDENTIFIER);
		while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
		return imp;
	}

	imp->path.root = lit_at(current()).literal._str;
	imp->path.full = lit_at(current()).literal._str;
	// printf("full path: %.*s\n", imp->path.full.len, imp->path.full.c_ptr);

	// check for subpaths
	if(check_next(T_DOT)) {
		imp->path.has_subpaths = 1;

		while(match(T_DOT)) {
			if(!match(T_IDENTIFIER)) {
				imp->had_error = 1;
				imp->err = parse_error(T_IDENTIFIER);
				// for now just fast forward until either the next import keyword or the next ';'
				while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
				return imp;
			}
			imp->path.subpath[imp->path.cur_subpath] = lit_at(current()).literal._str; 
			imp->path.full.len += lit_at(current()).literal._str.len + 1;
			imp->path.cur_subpath++;
		}
	}

	if(match(T_AS)) {
		if(!match(T_IDENTIFIER)) {
			imp->had_error = 1;
			imp->err = parse_error(T_IDENTIFIER);
			while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
			return imp;
		}
		imp->has_alias = 1;
		imp->alias = lit_at(current()).literal._str;
	}

	if (!match(T_SEMI)) {
		imp->had_error = 1;
		imp->err = parse_error(T_SEMI);
		while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume(); 
		return imp;
	}
	return imp;
}

static struct NodeFuncDef *fn_def(void) {

	NodeFuncDef *fn = malloc(sizeof(NodeFuncDef));
	fn->had_error = 0;
	
	// match type
	if(match_range(T_UBYTE, T_BOOL) ||match(T_IDENTIFIER)) {
		fn->return_type = current_tok().type;
	} else {
		fn->had_error = 1;
		// change this, T_TYPE means something totally different, but am just leaving
		// this here as is just to help me with debugging;
		fn->err = parse_error(T_TYPE);
		return fn;
	}

	// match identifier
	if (match(T_IDENTIFIER)) {
		fn->name = lit_at(current()).literal._str;
	} else {
		// error
		fn->had_error = 1;
		fn->err = parse_error(T_IDENTIFIER);
		return fn;
	}

	// parse params
	if(match(T_PAREN_OPEN)) {
		while(!match(T_PAREN_CLOSE)) {

		}
		// error
	}

	if(match(T_ARROW)) {
		fn->expr = expr();
	}
	// parse block
	else if(match(T_BRACE_OPEN)) {
		fn->body = block_expr();
	} else {
		// TODO: support proper "OR" type errors;
		fn->had_error = 1;
		fn->err = parse_error(T_ARROW);
		return fn;
	}
	return fn;
}

static struct NodeStructDef *struct_def(void) {
	return NULL;
}

static struct NodeGlobalDef *global_def(void) {
	return NULL;
}

struct ParseError parse_error(token_type expected) {
	return (ParseError) { .expected = expected, .got = peek() };
}

void print_ast(NodeProg node)
{
	for ( int i = 0; i < node.children->current; i++ ) {
		NodeItem *cur = node.children->data[i];
		switch(cur->type) {
			case I_UNKNOWN: printf("Unknown!\n"); break;
			case I_FN_DEF: printf("fn def\n"); break;
			case I_IMPORT_DEF: {
				if(cur->imp_def->had_error){ 
					printf(
						"[%i:%i] Error: Unexpected token %s, expected %s\nhere: %.*s\n",
						cur->imp_def->err.got.line,
						cur->imp_def->err.got.chr_index,
						token_to_str(cur->imp_def->err.expected),
						token_to_str(cur->imp_def->err.expected),
						cur->imp_def->err.got.source.len,
						cur->imp_def->err.got.source.c_ptr
						);
				break;
				}
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

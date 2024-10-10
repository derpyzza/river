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

struct NodeProg *parse_tokens( token_array_s *tokens, file_s src ) 
{ 
	init_parser(src, tokens);

	struct NodeProg *node = program();
	
	// NodeProg* node = malloc(sizeof(struct NodeProg));
	// node->expr = parse_expr();

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
		// printf("next: %s\n", token_to_str(peek().type));
		NodeItem* item = n_item();
		if(item->type != I_UNKNOWN) {
			vec_push(node->children, item);
		}
	}
	return node;
}

static struct NodeItem *n_item() {
	NodeItem* item = malloc(sizeof(struct NodeItem));
	item->type = I_UNKNOWN;

	// if (match(T_IMPORT)) {
	// 	item->imp_def = imp_def();
	// 	// if(item->imp_def->had_error) item->type = I_ERROR;
	// 	item->type = I_IMPORT_DEF;
	// 	return item;
	// }

	if (match(T_FN)) {
		item->type = I_FN_DEF;
		item->fn_def = fn_def();
		return item;
	}

	// if (match(T_STRUCT)) {
	// 	item->type = I_STRUCT_DEF;
	// 	item->struct_def = struct_def();
	// 	return item;
	// }
	//	
	//
	// if (match(T_CONST)) {
	// 	item->type = I_GLOBAL_DEF;
	// 	item->global_def = global_def();
	// 	return item;
	// }	

	return item;
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
		while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		return fn;
	}

	// match identifier
	if (match(T_IDENTIFIER)) {
		fn->name = lit_at(current()).literal._str;
	} else {
		// error
		fn->had_error = 1;
		fn->err = parse_error(T_IDENTIFIER);
		while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		return fn;
	}

	// parse params
	// if(match(T_PAREN_OPEN)) {
	// 	while(!match(T_PAREN_CLOSE)) {
	//
	// 	}
	// 	// error
	// }

	if(match(T_FAT_ARROW)) {
		fn->expr = parse_expr();
	}
	// parse block
	// else if(match(T_BRACE_OPEN)) {
	// 	fn->body = block_expr();
	// } 
	else {
		// TODO: support proper "OR" type errors;
		fn->had_error = 1;
		fn->err = parse_error(T_FAT_ARROW);
		while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		return fn;
	}

	if(!match(T_SEMI)) {
		fn->had_error = 1;
		fn->err = parse_error(T_SEMI);
		while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		return fn;
	}
	return fn;
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

	imp->path.root = current_tok().source;
	imp->path.full = current_tok().source;
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
			imp->path.subpath[imp->path.cur_subpath] = current_tok().source; 
			imp->path.full.len += current_tok().source.len + ( current_tok().source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
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
	// print_expr(node.expr);
	for ( int i = 0; i < node.children->current; i++ ) {
		NodeItem *cur = node.children->data[i];
		switch(cur->type) {
			case I_UNKNOWN: printf("Unknown!\n"); break;
			case I_FN_DEF: {
				if (cur->fn_def->had_error) {
					printf(
						"[%i:%i] Error: Unexpected token %s, expected %s\nhere: %.*s\n",
						cur->fn_def->err.got.line,
						cur->fn_def->err.got.chr_index,
						token_to_str(cur->fn_def->err.got.type),
						token_to_str(cur->fn_def->err.expected),
						cur->fn_def->err.got.source.len,
						cur->fn_def->err.got.source.c_ptr
						);
					break;
				}
				printf("<fn def> name: %.*s, return: %s, body:\n\t",
						cur->fn_def->name.len,
						cur->fn_def->name.c_ptr,
						token_to_str(cur->fn_def->return_type)
						);
				print_expr(cur->fn_def->expr);
			}
			break;
			case I_IMPORT_DEF: {
				if(cur->imp_def->had_error){ 
					printf(
						"[%i:%i] Error: Unexpected token %s, expected %s\nhere: %.*s\n",
						cur->imp_def->err.got.line,
						cur->imp_def->err.got.chr_index,
						token_to_str(cur->imp_def->err.got.type),
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

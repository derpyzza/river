#include <stdio.h>
#include <string.h>

#include "../scanner.h"
#include "../utils.h"
#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

// Forward declarations so that i can rearrange the definitions as i wish
static struct node *n_item(void);
static struct node *global_def(void);
static struct node *imp_def(void);
static struct node *fn_def(void);
static struct node *struct_def(void);
static struct node *type_def(void);
static struct node *enum_def(void);
static struct node *union_def(void);
static struct node *stmt(void);
static struct node *var_stmt(void);
static struct node *expr_stmt(void);

struct node *parse_tokens( token_array_s *tokens, file_s src ) 
{ 
	init_parser(src, tokens);

	struct node *node = new_node(N_PROG);
	node->children = vec_of(struct node*, 16);

	while(!match(T_EOF)) {
		struct node* item = n_item();
		if(item != NULL) {
			vec_push(node->children, item);
		} else {
			printf("Error: Unknown node type encountered\n");
		}
	}
	return node;
}

// static node_s *parser_panic(node_s* node, token_type expected, token_type until) {
// 	error_unexpected(node, expected);
// 	while(!check_next(T_EOF) && !check_next(until)) consume();
// 	node->type = N_NONE;
// 	return node;
// }

static struct node *n_item() {
	if (match(T_IMPORT)) {
		return imp_def();
	}

	if (match(T_FN)) {
		return fn_def();
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

	return NULL;
}

static struct node *fn_def(void) {

	struct node* fn = new_node(N_FUNC_DEF);

	// match type
	if(match_range(T_UBYTE, T_BOOL) ||match(T_IDENTIFIER)) {
		fn->type = current_tok().type;
	} else {
		// fn->had_error = 1;
		// // change this, T_TYPE means something totally different, but am just leaving
		// // this here as is just to help me with debugging;
		// fn->err = parse_error(T_TYPE);
		// while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		// return fn;
	}

	// match identifier
	if (match(T_IDENTIFIER)) {
		fn->name = lit_at(current()).literal._str;
	} else {
		// error
		// fn->had_error = 1;
		// fn->err = parse_error(T_IDENTIFIER);
		// while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		// return fn;
	}

	// parse params
	// if(match(T_PAREN_OPEN)) {
	// 	while(!match(T_PAREN_CLOSE)) {
	//
	// 	}
	// 	// error
	// }

	if(match(T_FAT_ARROW)) {
		fn->body = parse_expr();
	}
	// parse block
	// else if(match(T_BRACE_OPEN)) {
	// 	fn->body = block_expr();
	// } 
	else {
		// TODO: support proper "OR" type errors;
		// fn->had_error = 1;
		// fn->err = parse_error(T_FAT_ARROW);
		// while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		// return fn;
	}

	if(!match(T_SEMI)) {
		// fn->had_error = 1;
		// fn->err = parse_error(T_SEMI);
		// while(!check_next(T_EOF) && !check_next(T_FN)) consume();
		// return fn;
	}
	return fn;
}

static struct node *imp_def(void) {
	struct node *imp = new_node(N_IMPORT);

	if (!match(T_IDENTIFIER)) 
	{
		// error
		// imp->had_error = 1;
		// imp->err = parse_error(T_IDENTIFIER);
		// while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
		// return imp;
	}

	imp->path.root = current_tok().source;
	imp->path.full = current_tok().source;
	// printf("full path: %.*s\n", imp->path.full.len, imp->path.full.c_ptr);

	// check for subpaths
	if(check_next(T_DOT)) {
		imp->path.has_subpaths = 1;

		while(match(T_DOT)) {
			if(!match(T_IDENTIFIER)) {
				// imp->had_error = 1;
				// imp->err = parse_error(T_IDENTIFIER);
				// // for now just fast forward until either the next import keyword or the next ';'
				// while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
				// return imp;
			}
			imp->path.subpath[imp->path.cur_subpath] = current_tok().source; 
			imp->path.full.len += current_tok().source.len + ( current_tok().source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
			imp->path.cur_subpath++;
		}
	}

	if(match(T_AS)) {
		if(!match(T_IDENTIFIER)) {
			// imp->had_error = 1;
			// imp->err = parse_error(T_IDENTIFIER);
			// while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume();
			// return imp;
		}
		imp->has_name = 1;
		imp->name = lit_at(current()).literal._str;
	}

	if (!match(T_SEMI)) {
		// imp->had_error = 1;
		// imp->err = parse_error(T_SEMI);
		// while(!check_next(T_EOF) && !check_next(T_IMPORT)) consume(); 
		// return imp;
	}
	return imp;
}

struct node *new_node(enum node_tag type) {
	struct node* node = malloc(sizeof(struct node));
	if (node == NULL) {
		printf("Error, could not malloc new node\n");
		exit(-1);
	}
	node->tag = type;
	return node;
}

struct ParseError parse_error(token_type expected) {
	return (ParseError) { .expected = expected, .got = peek() };
}

void print_ast(struct node node)
{
	for ( int i = 0; i < node.children->current; i++ ) {
		struct node *cur = node.children->data[i];
		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_FUNC_DEF: {
				// if (cur->fn_def->had_error) {
				// 	printf(
				// 		"[%i:%i] Error: Unexpected token %s, expected %s\nhere: %.*s\n",
				// 		cur->fn_def->err.got.line,
				// 		cur->fn_def->err.got.chr_index,
				// 		token_to_str(cur->fn_def->err.got.type),
				// 		token_to_str(cur->fn_def->err.expected),
				// 		cur->fn_def->err.got.source.len,
				// 		cur->fn_def->err.got.source.c_ptr
				// 		);
				// 	break;
				// }
				printf("<fn def> name: %.*s, return: %s, body:\n\t",
						cur->name.len,
						cur->name.c_ptr,
						token_to_str(cur->type)
						);
				print_expr(cur->body);
			}
			break;
			case N_IMPORT: {
				// if(cur->imp_def->had_error){ 
				// 	printf(
				// 		"[%i:%i] Error: Unexpected token %s, expected %s\nhere: %.*s\n",
				// 		cur->imp_def->err.got.line,
				// 		cur->imp_def->err.got.chr_index,
				// 		token_to_str(cur->imp_def->err.got.type),
				// 		token_to_str(cur->imp_def->err.expected),
				// 		cur->imp_def->err.got.source.len,
				// 		cur->imp_def->err.got.source.c_ptr
				// 		);
				// break;
				// }
				substr_s root = cur->path.root;
				printf("<imp def> path: %.*s, root: %.*s", 
						cur->path.full.len,
						cur->path.full.c_ptr,
						root.len, root.c_ptr
						);
				if (cur->has_name) 
					printf(", alias: %.*s",
						cur->name.len,
						cur->name.c_ptr
							);
				printf("\n");
				break;
			}
		}
	}
}

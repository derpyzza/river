#include <stdio.h>
#include <stdlib.h>
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
			if(item->tag == N_NODE_LIST) {
				for(int i = 0; i < item->children->current; i++) {
					vec_push(node->children, item->children->data[i]);
				}
			}
			else vec_push(node->children, item);
		} 
	}

	if(check_errors() == 0) {
		return node;
	}
	else return NULL;
}

static struct node *var_assign() {
	struct node* node = new_node(N_VAR);

	if (match(T_IDENTIFIER)) {
		node->name = lit_at(current())->literal._str;	
	}
	match(T_EQUAL_SIGN);
	if( match(T_INTEGER_LITERAL)) {
		node->value = lit_at(current())->literal._int;
	}
	return node;
}

static struct node *global_var() {

	struct node* node = new_node(N_NODE_LIST);
	int has_commas = 0;
	struct node* assign = var_assign();

	if(peek().type == T_COMMA) vec_push(node->children, assign);
	while(match(T_COMMA)) {
		has_commas = 1;
		vec_push(node->children, var_assign());	
	}
	match(T_SEMI);

	if (has_commas)
		return node;
	else return assign;
}

static void panic_cat(token_type expected, tokcat until) {
	error_unexpected_tok(peek(), expected);
	while(!check_next(T_EOF) && !check_next_cat(until)) consume();
}

static void panic(token_type expected, token_type until) 
{
	error_unexpected_tok(peek(), expected);
	while(!check_next(T_EOF) && !check_next(until)) consume();
}

static struct node *n_item() {

	if (match(T_IMPORT)) {
		return imp_def();
	}

	if (match(T_INT)) {
		return global_var();
	}

	// if (match(T_FN)) {
	// 	return fn_def();
	// }

	// skip unknown characters for now *shrug*
	// maybe panic here?
	// dunno
	consume();
	return NULL;

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
}

static struct node *fn_def(void) {

	struct node* fn = new_node(N_FUNC_DEF);

	// match type
	if(match_range(T_UBYTE, T_BOOL) || match(T_IDENTIFIER)) {
		fn->type = current_tok().type;
	} else {
		panic(T_INT, T_SEMI);
	}

	// match identifier
	if (match(T_IDENTIFIER)) {
		fn->name = lit_at(current())->literal._str;
	} else {
		panic(T_IDENTIFIER, T_SEMI);
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
		panic(T_FAT_ARROW, T_SEMI);
	}

	if(!match(T_SEMI)) {
		panic_cat(T_SEMI, TC_KEYWORD);
	}
	return fn;
}

static struct node *imp_def(void) {
	struct node *imp = new_node(N_IMPORT);

	if (!match(T_IDENTIFIER)) 
	{
		panic(T_IDENTIFIER, T_SEMI);
	}

	imp->path.root = current_tok().source;
	imp->path.full = current_tok().source;

	// check for subpaths
	if(check_next(T_DOT)) {
		imp->path.has_subpaths = 1;

		while(match(T_DOT)) {
			if(!match(T_IDENTIFIER)) {
				panic(T_IDENTIFIER, T_SEMI);
			}
			imp->path.subpath[imp->path.cur_subpath] = current_tok().source; 
			imp->path.full.len += current_tok().source.len + ( current_tok().source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
			imp->path.cur_subpath++;
		}
	}

	if(match(T_AS)) {
		if(!match(T_IDENTIFIER)) {
			panic(T_IDENTIFIER, T_SEMI);
		}
		imp->has_name = 1;
		imp->name = lit_at(current())->literal._str;
	}

	if (!match(T_SEMI)) {
		panic_cat(T_SEMI, TC_KEYWORD);
	}
	return imp;
}

struct node *new_node(enum node_tag type) {
	size_t size = sizeof(struct node);
	struct node* node = malloc(size);
	if (node == NULL) {
		printf("Error, could not malloc new node\n");
		exit(-1);
	}
	node->tag = type;
	switch(node->tag) {
		case N_NODE_LIST:
		case N_BLOCK:
			node->children = vec_of(struct node*, 8);
	}
	return node;
}

// struct ParseError parse_error(token_type expected) {
// 	return (ParseError) { .expected = expected, .got = peek() };
// }

void print_ast(struct node node)
{
	for ( int i = 0; i < node.children->current; i++ ) {
		struct node *cur = node.children->data[i];
		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_VAR: {
					printf("<var> name: %.*s, type: int, value: %i\n",
						cur->name.len,
						cur->name.c_ptr,
						(int)cur->value
					);
				}
			break;
			case N_FUNC_DEF: {
				printf("<fn def> name: %.*s, return: %s, body:\n\t",
						cur->name.len,
						cur->name.c_ptr,
						token_to_str(cur->type)
						);
				print_expr(cur->body);
			}
			break;
			case N_IMPORT: {
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

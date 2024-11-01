#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "parser.h"
#include "../utils.h"
#include "../scanner.h"
#include "parser_utils.h"

// Forward declarations so that i can rearrange the definitions as i wish
static struct Node *top_level(void);
static struct Node *global_var(void);
static struct Node *import_def(void);
static struct Node *const_def(void);
static struct Node *fn_def(void);
static struct Node *struct_def(void);
static struct Node *type_def(void);
static struct Node *enum_def(void);
static struct Node *union_def(void);
static struct Node *stmt(void);
static struct Node *var_stmt(void);
static struct Node *expr_stmt(void);


// static void panic_cat(TokenType expected, CatType until, const char * file, const int line) {
// 	error_unexpected_tok(current()+1, expected, file, line);
// 	while(!check_next(T_EOF) && !check_next_cat(until)) consume();
// }

enum PanicType {
	P_CAT = 0,
	P_TOK = 1
};

static void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line) {
	error_unexpected(parser.current + 1, expected, is_expected_tok, file, line);
	if (is_expected_tok)
		while(!check_next(T_EOF) && !check_next(until)) consume();
	else
		while(!check_next(T_EOF) && !check_next_cat(until)) consume();
}

#define PANIC(e, et, u, ut) panic(e, et, u, ut, __FILE__, __LINE__)
// #define PANIC_CAT(t, u) panic_cat(t, u, __FILE__, __LINE__)

struct Node *stmt(void) {
	struct Node *stmt = parse_expr();
	match(T_SEMI);
	return stmt;
}


struct Node *parse_tokens( TokenArray *tokens, File *src ) 
{ 
	init_parser(src, tokens);

	// malloc a big chunk of memory at the start and just use that
	// setting the number of nodes to be equal to the number of tokens in a file is 
	// super duper wasteful, since the number of nodes will always be a lot less than
	// the number of tokens, as every node is made up of one or more tokens.
	// still, it's an alright number to target, and it makes sure that we don't have to 
	// reallocate at all. This might cause problems with big files on machines with lesser 
	// amounts of RAM to spare, but ehhhhh.
	// we'll burn that bridge when we get to it :)
	// struct Node* nodeList = malloc(sizeof(struct Node) * parser.tokens->current_token); 

	struct Node *node = new_node(N_PROG);
	node->children = vec_of(struct Node*, 16);

	while(!match(T_EOF)) {
		struct Node* item = top_level();
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


static struct Node *top_level(void) {

	if (match(T_IMPORT)) {
		return import_def();
	}

	if (match(T_CONST) || peek().type == T_CONST) {
		return const_def();
	}

	// should redo my function / variable parsing strategy here.
	// i should first try to match on keywords that i know for sure cannot be followed by one or the other,
	// and if i don't fine said keywords, i should then search for the `TYPE ID ( '=' / '=>' )` pattern.
	// for instance, the 'pub' keyword is guaranteed to never be a variable definition, it'll always be either a function, struct, enum, union, interface or type.
	// similarly, the `let` keyword will never ever be followed up with anything other than a variable declaration.

	// check for functions or variables. These require a lookahead to differenciate between them.
	if ( match(T_LET) ) {
		printf("matched LET\n");
		return global_var();	
	} 
	if ( check_type_token() ) {
		return (check_next_n(3, T_PAREN_OPEN) || check_next_n(3, T_FAT_ARROW)) 
		? fn_def() : global_var(); 
	}

	if (match(T_STRUCT)) {
		return struct_def();
	}

	// skip unknown characters for now *shrug*
	// maybe panic here?
	// dunno
	consume();
	return NULL;
}


static struct Node *import_def(void) {
	struct Node *imp = new_node(N_IMPORT);

	if (!match(T_IDENTIFIER)) 
	{
		PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
		return NULL;
	}
	imp->path.root = current_tok()->source;
	imp->path.full = current_tok()->source;

	// check for subpaths
	if(check_next(T_DOT)) {
		imp->path.has_subpaths = 1;

		while(match(T_DOT)) {
			if(!match(T_IDENTIFIER)) {
				PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
				return NULL;
			}
			imp->path.subpath[imp->path.cur_subpath] = current_tok()->source; 
			imp->path.full.len += current_tok()->source.len + ( current_tok()->source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
			imp->path.cur_subpath++;
		}
	}

	if(match(T_AS)) {
		if(!match(T_IDENTIFIER)) {
			PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
			return NULL;
		}
		imp->has_name = 1;
		imp->name_id = current_tok()->literal_id;
	}

	if (!match(T_SEMI)) {
		PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);
		return NULL;
	}
	return imp;
}


// const values are compile time evaluated constant expressions
static struct Node* const_def(void) {
	struct Node* node =new_node(N_CONST);
	match_type_token();
	node->type = current_tok()->type;
	match(T_IDENTIFIER);
	node->name_id = current_tok()->literal_id;
	match(T_EQUAL_SIGN);
	match_cat(TC_LITERAL);
	node->value = cur_lit();
	match(T_SEMI);
	return node;
}

static struct Node *var_assign(void) {
	struct Node* node = new_node(N_VAR);

	// pull out type
	if (prev().type != T_LET) {
		consume();
		node->type = current_tok()->type;
	}

	printf("current token: %s, prev token: %s, next token: %s\n", 
	       token_to_str(current_tok()->type),
	       token_to_str(prev().type),
	       token_to_str(peek().type)
	     );

	// pull out identifier
	consume();
	node->name_id = current_tok()->literal_id;	
	printf("val name: %.*s\n", (int)lit_at(node->name_id)->value.string.len, lit_at(node->name_id)->value.string.c_ptr);

	if (!match(T_EQUAL_SIGN)) PANIC(T_EQUAL_SIGN, P_TOK, TC_KEYWORD, P_CAT);
	if( !match_cat(TC_LITERAL)) PANIC(TC_LITERAL, P_CAT, TC_KEYWORD, P_CAT);
	node->type = current_tok()->type;
	node->value = cur_lit();
	return node;
}

static struct Node *global_var(void) {

	struct Node* node = new_node(N_NODE_LIST);
	int has_commas = 0;
	struct Node* assign = var_assign();

	if(peek().type == T_COMMA) vec_push(node->children, assign);
	while(match(T_COMMA)) {
		has_commas = 1;
		vec_push(node->children, var_assign());
	}
	if(!match(T_SEMI)) PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);

	if (has_commas)
		return node;
	else return assign;
}

// takes in the `type` of the variable as an argument, since the type could appear
// a lot further before the current variable, so you can't just peek backwards to check it out
// e.g:
//     int x, y, z, q, w, e, 'r', u, i;
// If we're parsing the item 'r' for example, we'd have to do about 12 tokens of look-back
// to know what the type is. instead, the type is parsed and stored when it's encountered, and
// then passed in to any variable that is being parsed.
static struct Node* struct_item(TokenType type) {
	struct Node* item = new_node(N_STRUCT_FIELD);
	item->type = type;
	match(T_IDENTIFIER);
	item->name_id = current_tok()->literal_id;
	if(match(T_EQUAL_SIGN)) {
		match_cat(TC_LITERAL);
		item->value = cur_lit();
	}
	return item;
}

static struct Node* struct_field() {
	TokenType field_type = match_type_token();
	struct Node* item = struct_item(field_type);

	if(peek().type == T_COMMA) {

		struct Node* list = new_node(N_NODE_LIST);
		vec_push(list->children, item);

		// match all items
		while(match(T_COMMA)) vec_push(list->children, struct_item(field_type));		

		return list;
	}

	return item;
}

static struct Node *struct_def(void) {
	struct Node* node = new_node(N_STRUCT_DEF);
	match(T_IDENTIFIER);
	node->name_id = current_tok()->literal_id;
	match(T_BRACE_OPEN);
	while(!match(T_BRACE_CLOSE)) {
		struct Node* item = struct_field();		
		if(item) {
			if (item->tag == N_NODE_LIST) {
				for(int i = 0; i < item->children->current; i++) 
					vec_push(node->children, item->children->data[i]);
			}
			else vec_push(node->children, item);
		}
		match(T_SEMI);
	}
	return node;
}

static struct Node *fn_def(void) {
	struct Node* fn = new_node(N_FUNC_DEF);
	// we are guaranteed to atleast have the following tokens if we've entered this function:
	// a type token, an identifier, and either a '(' or a '=>'
	// so we don't need to match the first two, we can just directly consume them and store
	// their data;
 
	// match type
	consume();
	fn->type = current_tok()->type;

	// match identifier
	consume();
	fn->name_id = current_tok()->literal_id;

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
	else {
		// TODO: support proper "OR" type errors;
		PANIC(T_FAT_ARROW, P_TOK, T_SEMI, P_TOK);
	}

	if(!match(T_SEMI)) {
		PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);
	}
	return fn;
}

struct Node *new_node(enum NodeTag type) {
	size_t size = sizeof(struct Node);
	struct Node* node = malloc(size);
	if (node == NULL) {
		printf("Error, could not malloc new node\n");
		exit(-1);
	}
	node->tag = type;
	node->has_name = 0;
	node->is_stmt = 0;
	switch(node->tag) {
		default: break;
		case N_NODE_LIST:
		case N_STRUCT_DEF:
		case N_BLOCK:
			node->children = vec_of(struct Node*, 8);
	}
	return node;
}

// struct ParseError parse_error(token_type expected) {
// 	return (ParseError) { .expected = expected, .got = peek() };
// }

void print_lit(Literal *lit) {
	if (lit)
		switch (lit->type) {
			case LIT_NONE: printf("<none>\n"); break;
			case LIT_FALSE: printf("<false>\n"); break;
			case LIT_TRUE: printf("<true>\n"); break;
			case LIT_CHAR: printf("'%c'\n", lit->value.character); break;
			case LIT_INT: printf("%lli\n", lit->value.integer); break;
			case LIT_FLOAT: printf("%lf\n", lit->value.floating);
			case LIT_STRING: printf("\"%.*s\"\n", (int)lit->value.string.len, lit->value.string.c_ptr);
		}
	else printf("<Uninitialized>\n");
}

void print_ast(struct Node node)
{
	for ( int i = 0; i < node.children->current; i++ ) {
		struct Node *cur = node.children->data[i];
		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_VAR: {
					printf("<var> name: %.*s, type: int, value: ",
						(int)lit_at(cur->name_id)->value.string.len,
						lit_at(cur->name_id)->value.string.c_ptr
					);
					print_lit(cur->value);
				}
			break;
			case N_STRUCT_FIELD: {
				printf("\t<field> name: %.*s, type: %s, value: ",

					(int)lit_at(cur->name_id)->value.string.len,
					lit_at(cur->name_id)->value.string.c_ptr,
					token_to_str(cur->type)
				);
				print_lit(cur->value);
				break;
			} 				
			case N_STRUCT_DEF: {
				printf("<struct def> name: %.*s, body:\n", 

					(int)lit_at(cur->name_id)->value.string.len,
					lit_at(cur->name_id)->value.string.c_ptr
				);
				print_ast(*cur);
				break;
			}
			case N_FUNC_DEF: {
				printf("<fn def> name: %.*s, return: %s, body:\n\t",
						(int)lit_at(cur->name_id)->value.string.len,
						lit_at(cur->name_id)->value.string.c_ptr,
						token_to_str(cur->type)
						);
				print_expr(cur->body);
				printf("\n");
			}
			break;
			case N_CONST: {
					printf("<const> name: %.*s, type: %s, body:",
						(int)lit_at(cur->name_id)->value.string.len,
						lit_at(cur->name_id)->value.string.c_ptr,
						token_to_str(cur->type)
					);
					print_lit(cur->value);
			}
			break;
			case N_IMPORT: {
				String root = cur->path.root;
				printf("<imp def> path: %.*s, root: %.*s", 
						(int)cur->path.full.len,
						cur->path.full.c_ptr,
						(int)root.len, 
						root.c_ptr
						);
				if (cur->has_name) 
					printf(", alias: %.*s",
						(int)lit_at(cur->name_id)->value.string.len,
						lit_at(cur->name_id)->value.string.c_ptr);
				printf("\n");
				break;
			}
		}
		// printf("\n");
	}
}

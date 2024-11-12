#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "parser.h"
#include "../utils.h"
#include "../scanner.h"
#include "../diagnostics.h"

// Forward declarations so that i can rearrange the definitions as i wish
static struct Node *top_level(void);
static struct Node *fn_def(void);
// static struct Node *global_var(void);
// static struct Node *import_def(void);
// static struct Node *const_def(void);
// static struct Node *struct_def(void);
// static struct Node *type_def(void);
// static struct Node *enum_def(void);
// static struct Node *union_def(void);
// static struct Node *stmt(void);
// static struct Node *var_stmt(void);
// static struct Node *expr_stmt(void);

struct Parser {
	size current;
	bool had_error;
	File *source; // current file being parsed
	VecParseErrors *errors;
	VecToken *tokens;
};

// spooky global parser struct
static struct Parser parser;

void init_parser(File *source, VecToken *tokens) {
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
	parser.had_error = False;
	parser.errors = NULL;
}

void consume(void) { 
	parser.current++;
}

size current(void) {
	 return parser.current; 
}

Token current_tok(void) {
	return parser.tokens->data[parser.current]; 
}

Token *token_at(size id) {
	if (id >= parser.tokens->current ) {
		return NULL;
	}
	return &parser.tokens->data[id];
}

struct Token peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current ) {
		return token_eof();
	}
	return parser.tokens->data[parser.current + offset];
}

struct Token peek(void) { 
	return peek_n(1); 
}

struct Token prev_n(int offset) {
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->data[parser.current - offset];
}

struct Token prev(void) { 
	return prev_n(1);
}

int check_next_n(int offset, int expected) { 
	// printf("T: %i\n", peek_n(offset).type == expected);
	return (peek_n(offset).type == expected); 
}

int check_next (int expected) { 
	// printf("next is: %s\n", token_to_str(peek().type));
	return check_next_n(1, expected);
}

int check_next_cat ( int cat ) {
	return (peek().cat == cat); 
}

void error_unexpected( int got, int expected, bool is_tok, const char * file, const int line ) {
	if (!parser.had_error) {
		parser.had_error = 1;
		parser.errors = new_vec_ParseErrors(8);
	}
	struct ParseError *err = (ParseError*)malloc(sizeof(struct ParseError));
	err->type = expected;
	err->got = got;
	err->debug_file = file;
	err->debug_line = line;
	err->is_tok = is_tok;
	vec_push_ParseErrors(parser.errors, *err);
}

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

void report_error(ParseError *error) {
	printf("[%sCOMPILER_DEBUG%s]: error emitted from <%s:%i>\n", 
	      TERM_LRED,
       	TERM_RESET, 
				error->debug_file,
				error->debug_line);

	if ( error->is_tok ) {
		printf("%s:%i:%i: ERROR Unexpected token; expected '%s', got '%.*s' (%s)\n",
			parser.source->path,
			(int)parser.tokens->data[error->got].line,
			parser.tokens->data[error->got].chr_index,
			token_to_str((TokenType)error->type),
			(int)parser.tokens->data[error->got].span.len,
			parser.tokens->data[error->got].span.c_ptr,
			token_to_str(parser.tokens->data[error->got].type));
	} else {
		printf("%s:%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
			parser.source->path,
			(int)parser.tokens->data[error->got].line,
			parser.tokens->data[error->got].chr_index,
			tokcat_to_str((TokCat)error->type),
			(int)parser.tokens->data[error->got].span.len,
			parser.tokens->data[error->got].span.c_ptr,
			token_to_str(parser.tokens->data[error->got].type));
	}
}

bool check_errors(void) {
	if (parser.had_error) {
		for (int i = 0; i < parser.errors->current; i++) {
			report_error(&parser.errors->data[i]);
		}
		return True;
	} else return False;
}

int match_cat(int cat) {
	if ( tok_to_cat(peek().type) == cat ) { 
		consume();
		return 1; 
	} 
	return 0;
}

int match_range(int start, int end) 
{
	if (peek().type >= start && peek().type <= end) {
		consume();
		return 1;
	}
	return 0;
}

int match( TokenType expected) {
	if (peek().type == expected) {
		consume();
		return 1;
	}
	return 0;
}

// struct Node *stmt(void) {
// 	struct Node *stmt = parse_expr();
// 	match(T_SEMI);
// 	return stmt;
// }


struct Node *parse_tokens( VecToken *tokens, File *src ) 
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
	node->children = new_vec_Node(8);

	while(!match(T_EOF)) {
		struct Node* item = top_level();
		if(item != NULL) {
			if(item->tag == N_NODE_LIST) {
				for(int i = 0; i < item->children->current; i++) {
					vec_push_Node(node->children, item->children->data[i]);
				}
			}
			else vec_push_Node(node->children, item);
			// print_ast(*node);
		} 

	}

	if(check_errors() == 0) {
		return node;
	}
	else return NULL;
}


static struct Node *top_level(void) {

	if(match(T_FUN)) return fn_def();

	// skip unknown characters for now *shrug*
	// maybe panic here?
	// dunno
	// printf("unknown node type: %s\n", token_to_str(current_tok().type));
	consume();
	return NULL;
}


// static struct Node *import_def(void) {
// 	struct Node *imp = new_node(N_IMPORT);

// 	if (!match(T_IDENTIFIER)) 
// 	{
// 		PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
// 		return NULL;
// 	}
// 	imp->path.root = current_tok().source;
// 	imp->path.full = current_tok().source;

// 	// check for subpaths
// 	if(check_next(T_DOT)) {
// 		imp->path.has_subpaths = 1;

// 		while(match(T_DOT)) {
// 			if(!match(T_IDENTIFIER)) {
// 				PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
// 				return NULL;
// 			}
// 			imp->path.subpath[imp->path.cur_subpath] = current_tok().source; 
// 			imp->path.full.len += current_tok().source.len + ( current_tok().source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
// 			imp->path.cur_subpath++;
// 		}
// 	}

// 	if(match(T_AS)) {
// 		if(!match(T_IDENTIFIER)) {
// 			PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
// 			return NULL;
// 		}
// 		imp->has_name = 1;
// 		imp->name_id = current_tok().literal_id;
// 	}

// 	if (!match(T_SEMI)) {
// 		PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);
// 		return NULL;
// 	}
// 	return imp;
// }


// const values are compile time evaluated constant expressions
// static struct Node* const_def(void) {
// 	struct Node* node =new_node(N_CONST);
// 	match_type_token();
// 	node->type = current_tok().type;
// 	match(T_IDENTIFIER);
// 	node->name_id = current_tok().literal_id;
// 	match(T_EQUAL);
// 	match_cat(TC_LITERAL);
// 	node->value = current_tok().literal_id;
// 	match(T_SEMI);
// 	return node;
// }

// static ^Node var_assign => {}
// fun int add (int x, y) = x + y;
// fun ^Node var_assign = {}
// let ^Node node = new Node();
// ^Node node = new Node();
// let node = new Node();

// static struct Node *var_assign(void) {
// 	struct Node* node = new_node(N_VAR);

// 	if (current_tok().type == T_LET) {

// 		if(!match(T_IDENTIFIER)) PANIC(T_IDENTIFIER, P_TOK, TC_KEYWORD, P_CAT);
// 		node->name_id = current_tok().literal_id;	
// 	} else {

// 		// pull out type
// 		consume();

// 		// the type can either be a set of built-in types,
// 		// or an identifier that corresponds to a user defined type
// 		// ( which can be one of a struct, an enum, a union, or a type alias ) 
// 		// 

// 		TokenType t = current_tok().type;
// 		if( t == T_IDENTIFIER )
// 			node->type_id = current_tok().literal_id;
// 		node->type = current_tok().type;

// 		// pull out identifier
// 		consume();
// 		node->name_id = current_tok().literal_id;
// 	}


//   if (!match(T_EQUAL))
//     PANIC(T_EQUAL, P_TOK, TC_KEYWORD, P_CAT);

// 	if( !match_cat(TC_LITERAL)) 
// 		PANIC(TC_LITERAL, P_CAT, TC_KEYWORD, P_CAT);
// 	node->value = current_tok().literal_id;
// 	return node;
// }

// static struct Node *global_var(void) {

// 	struct Node* node = new_node(N_NODE_LIST);
// 	int has_commas = 0;
// 	struct Node* assign = var_assign();

// 	if(peek().type == T_COMMA) vec_push(node->children, assign);
// 	while(match(T_COMMA)) {
// 		has_commas = 1;
// 		vec_push(node->children, var_assign());
// 	}
// 	if(!match(T_SEMI)) PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);

// 	if (has_commas)
// 		return node;
// 	else return assign;
// }

// takes in the `type` of the variable as an argument, since the type could appear
// a lot further before the current variable, so you can't just peek backwards to check it out
// e.g:
//     int x, y, z, q, w, e, 'r', u, i;
// If we're parsing the item 'r' for example, we'd have to do about 12 tokens of look-back
// to know what the type is. instead, the type is parsed and stored when it's encountered, and
// then passed in to any variable that is being parsed.
// static struct Node* struct_item(TokenType type) {
// 	struct Node* item = new_node(N_STRUCT_FIELD);
// 	item->type = type;
// 	match(T_IDENTIFIER);
// 	item->name_id = current_tok().literal_id;
// 	if(match(T_EQUAL)) {
// 		match_cat(TC_LITERAL);
// 		item->value = current_tok().literal_id;
// 	}
// 	return item;
// }

// static struct Node* struct_field(void) {
// 	TokenType field_type = match_type_token();
// 	struct Node* item = struct_item(field_type);

// 	if(peek().type == T_COMMA) {

// 		struct Node* list = new_node(N_NODE_LIST);
// 		vec_push(list->children, item);

// 		// match all items
// 		while(match(T_COMMA)) vec_push(list->children, struct_item(field_type));		

// 		return list;
// 	}

// 	return item;
// }

// static struct Node *struct_def(void) {
// 	struct Node* node = new_node(N_STRUCT_DEF);
// 	match(T_IDENTIFIER);
// 	node->name_id = current_tok().literal_id;
// 	match(T_BRACE_OPEN);
// 	while(!match(T_BRACE_CLOSE)) {
// 		struct Node* item = struct_field();		
// 		if(item) {
// 			if (item->tag == N_NODE_LIST) {
// 				for(int i = 0; i < item->children->current; i++) 
// 					vec_push(node->children, item->children->data[i]);
// 			}
// 			else vec_push(node->children, item);
// 		}
// 		match(T_SEMI);
// 	}
// 	return node;
// }

String * cur_tok_span() {
	return &parser.tokens->data[parser.current].span;
}

// function_def -> "fun" <ID> ( "(" <params_list> ")" )? ( "->" <type> )? "=" <expr> ";" .
static struct Node *fn_def(void) {
	struct Node* fn = new_node(N_FUNC_DEF);
	// we are guaranteed to atleast have the following tokens if we've entered this function:
	// a type token, an identifier, and either a '(' or a '=>'
	// so we don't need to match the first two, we can just directly consume them and store
	// their data;
 
	// match identifier
	if( match(T_IDENTIFIER) )
		fn->name = cur_tok_span();
	else printf("error, missing identifier");

	// parse params
	if(match(T_PAREN_OPEN)) {
		while(!match(T_PAREN_CLOSE)) {
			consume();
		}
		// error
	}

	
	if (match(T_ARROW)) {
		// match type
		if (match(T_IDENTIFIER)) {
			printf("Matched identifier: %.*s\n", cur_tok_span()->len, cur_tok_span()->c_ptr);
			fn->type = cur_tok_span();
		}
		else 
		 printf("ERROR: expected return type\n"), exit(-1);
	}

	if(match(T_EQUAL)) {
		fn->body = parse_expr();
	}
	else {
		// TODO: support proper "OR" type errors;
		PANIC(T_EQUAL, P_TOK, T_SEMI, P_TOK);
	}

	if(!match(T_SEMI)) {
		PANIC(T_SEMI, P_TOK, TC_KEYWORD, P_CAT);
	}
	return fn;
}

struct Node *new_node(enum NodeTag type) {
	struct Node* node = (Node *)malloc(sizeof(struct Node));
	if (node == NULL) {
		printf("Error, could not malloc new node\n");
		exit(-1);
	}
	node->tag = type;

	node->type
		= node->name 
		= node->value
		= NULL;

	node->op = T_NONE;

	node->rhs 
		= node->lhs 
		= node->cond 
		= node->els 
		= node->body 
		= node->init 
		= node->inc 
		= NULL;
	switch(node->tag) {
		default: break;
		case N_NODE_LIST:
		case N_STRUCT_DEF:
		case N_BLOCK:
			node->children = new_vec_Node(8);
	}
	return node;
}


// TODO: Probably a waste of effort, but consider writing a set of proper string builder functions for generating debug parser output.
void print_ast(struct Node *node, int level)
{
	if (!node) exit(-1);
	if (level == 0) printf("<root>\n");
	for ( int i = 0; i < node->children->current; i++ ) {
		printf("\t");
		struct Node *cur = (Node*)node->children->data[i];
		if (cur == NULL) continue;
		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_VAR: {
				printf("<var> name: %.*s, type: <TYPE>, rhs: ",
					cur->name->len,
					cur->name->c_ptr
				);
				print_expr(cur->rhs);
				printf("; \n");
				break;
			}
			case N_STRUCT_FIELD: {
				if (cur->rhs) {
					printf("<field> name: %.*s, type: %.*s, default value: ",
						cur->name->len,
						cur->name->c_ptr,
						cur->type->len,
						cur->type->c_ptr
					);
					print_expr(cur->rhs);
					printf("; \n");
				} else 
					printf("<field> name: %.*s, type: %.*s;\n",
						cur->name->len,
						cur->name->c_ptr,
						cur->type->len,
						cur->type->c_ptr
					);
				break;
			} 				
			case N_STRUCT_DEF: {
				printf("\n\t<STRUCT DEF>\n\t\tname: %.*s, body:\n", 
					cur->name->len,
					cur->name->c_ptr
				);
				print_ast(cur, 1);
				printf("\n");

				break;
			}
			case N_FUNC_DEF: {
				printf("<fn def> name: %.*s, return: %.*s, body: ",
					cur->name->len,
					cur->name->c_ptr,
					cur->type->len,
					cur->type->c_ptr
				);
				print_expr(cur->body);
				printf("; \n");

				break;
			}
			case N_CONST: {
					printf("<const> name: %.*s, type: %*.s, body:",
						cur->name->len,
						cur->name->c_ptr,
						cur->type->len,
						cur->type->c_ptr
					);
				print_expr(cur->rhs);
				printf(" ;\n");

				break;
			}
			case N_IMPORT: {
				String root = cur->path.root;
				printf("<imp def> path: %.*s, root: %.*s", 
						(int)cur->path.full.len,
						cur->path.full.c_ptr,
						(int)root.len, 
						root.c_ptr
						);
				if (cur->name) 
					printf(", alias: %.*s",
						cur->name->len,
						cur->name->c_ptr
					);
				printf("\n");
				break;
			}
		}
		// printf("\n");
	}
}

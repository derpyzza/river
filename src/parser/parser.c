#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "memory.h"
#include "parser-internal.h"

#include "expr.h"
#include "../scanner.h"


// Forward declarations so that i can rearrange the definitions as i wish
static struct Node *top_level(void);
static struct Node *fn_def(void);
static struct Node *var_def(void);
static struct Node *type_def(bool is_alias);
static struct Node *import_def(void);

void init_parser( dstr source, dbuf_token *tokens) {
	parser.node_tree = dbuf_new_Node(1024);
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
	parser.had_error = false;
	parser.errors = NULL;
}

Node * new_node(NodeTag type) {
	Node * node = &parser.node_tree->data[parser.node_tree->current];
	parser.node_tree->current ++;
	node->tag = type;
	node->type = NULL;
	node->iden = (dstr){0};
	node->value = (dstr) {0};

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
			node->children = dbuf_new_NodeList(8);
	}
	return node;
}

#define PANIC(e, et, u, ut) panic(e, et, u, ut, __FILE__, __LINE__)

bool match_type_token(void) {
	return false;
}

dbuf_Node *parse_tokens( dbuf_token *tokens, dstr src )  { 

	// Initialize parser state, parser struct found in src/parser/parser-internal.h
	init_parser(src, tokens);

	Node * n = new_node(N_TODO);
	dbuf_push_Node(parser.node_tree, *n);
	print_ast(parser.node_tree, 0);
	return 0;

	while(!match(T_EOF)) {
		struct Node* item = top_level();
		if(item != NULL) {
			if(item->children) {
				for(int i = 0; i < item->children->current; i++) {
					dbuf_push_Node(parser.node_tree, **item->children->data[i]);
				}
			} else {
				dbuf_push_Node(parser.node_tree, *item);
			}
		} 
	}

	if(check_errors() == 0) {
		return parser.node_tree;
	}
	else return NULL;
}


static Node* var_def(void) { return new_node(N_TODO); }
static Node* import_def(void) { return new_node(N_TODO); }
static Node* type_def(bool is_alias) { return new_node(N_TODO); }

static Node *top_level(void) {
	if(match(T_FUN)) return fn_def();
	if(match(T_LET)) return var_def();
	// if(match(T_TYPE)) return type_def(0);
	if(match(T_ALIAS)) return type_def(1);
	if(match(T_IMPORT)) return import_def();

	// skip unknown characters for now *shrug*
	// maybe panic here?
	// dunno
	// printf("unknown node type: %s\n", token_to_str(current_tok().type));
	consume();
	return NULL;
}

// Datatype * new_datatype(DatatypeTag tag) {
// 	Datatype * new = malloc(sizeof(Datatype));
// 	new->tag = tag;
// 	new->type = malloc(sizeof(DatatypeUnion));
// }


/* 
type tokens: 
  ( "^"* ("[" <int> "]")+ | ("[" "]")* ) "^"* IDEN ("," type)* ;

datatype -> primary_type ( "," primary_tab )* ;
primary_type ->

*/

// Datatype* parse_datatype_array(void) {}
// Datatype* parse_datatype_header(void) {}

// Datatype* parse_datatype_primary(void) {
// 	if(match(T_IDEN)) {
// 		// check if tuple
// 		if (check_next(',')) {
// 			Datatype* tuple = new_datatype(DATATYPE_TUPLE);
// 		}
// 	}
// }

// Datatype* parse_datatype(void) {
// 	Datatype* out = parse_datatype_primary();
// 	if(match(',')) {
// 		out->type->dt_tuple = parse_datatype_primary();
// 	} else if (match('|')) {
// 		out->type->dt_union = parse_datatype_primary();
// 	}
// }

// void match_params() {
	
// }

// function_def -> "fun" <ID> ( "(" <params_list> ")" )? ( "->" <type> )? "=" <expr> ";" .
// "fun" <ID> ( "::" <ID> )? ( "[" <generic> "]" )? ( "(" <params_list>? ")" )? ( "->" <type> )? ( "=" <expr> ";" | <block> )
static Node *fn_def(void) {
	Node* fn = new_node(N_FUNC_DEF); 

	// match identifier
	if( match_type_token() ) {
		fn->iden = *cur_tok_span();
	} else {
		printf("error, missing type");
		// PANIC(T_TYPE, P_CAT, ';', P_TOK);
	}

	// parse params
	if(match('(')) {
		while(!match(')')) {
			consume();
		}
		// error
	}

	
	if (match(T_ARROW)) {
		// match type
		if (match_type_token()) {
			// fn->type = *cur_tok_span();
		}
		else 
		 printf("ERROR: expected return type\n"), exit(-1);
	}

	if(match('=')) {
		fn->body = parse_expr();
	}
	else {
		// TODO: support proper "OR" type errors;
		PANIC('=', P_TOK, ';', P_TOK);
	}

	if( fn->body->tag != N_BLOCK && !match(';')) {
		PANIC(';', P_TOK, TC_KEYWORD, P_CAT);
	}
	printf("returning fn!\n");
	return fn;
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

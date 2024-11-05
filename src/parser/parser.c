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
static struct Node *global_var(void);
static struct Node *import_def(void);
static struct Node *const_def(void);
static struct Node *fn_def(void);
static struct Node *struct_def(void);
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
	Vec *errors;
	TokenArray *tokens;
};

// spooky global parser struct
static struct Parser parser;


void init_parser(File *source, TokenArray *tokens) {
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
	return parser.tokens->token_list[parser.current]; 
}

Token *token_at(size id) {
	if (id >= parser.tokens->current_token ) {
		return NULL;
	}
	return &parser.tokens->token_list[id];
}

// get literal stored in the literal id field of given token
struct Literal *lit_at_tok(size tok_id)
{
	if (tok_id >= parser.tokens->current_token) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return &(Literal){.type = LIT_NONE};
	}
	int lit_id = token_at(tok_id)->literal_id;	
	return &parser.tokens->literal_list[lit_id];
}

// get literal stored in the given id
struct Literal *lit_id(long lit_id){
	if (lit_id >= parser.tokens->current_literal) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return &(Literal){.type = LIT_NONE};
	}
	return &parser.tokens->literal_list[lit_id];
}

// shortcut to get the lit stored in the lit_id field of the current token
struct Literal *cur_lit(void) {
	return lit_at_tok(parser.current);
}

struct Token peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

struct Token peek(void) { 
	return peek_n(1); 
}

struct Token prev_n(int offset) {
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->token_list[parser.current - offset];
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

int check_type_token(void) {
	TokenType next = peek().type;
	if ((next >= T_UBYTE && next <= T_BOOL) || (next == T_IDENTIFIER)) {
		return 1;	
	}

	return 0;
}

int check_next_cat ( int cat ) {
	return (peek().cat == cat); 
}

void error_unexpected( int got, int expected, bool is_tok, const char * file, const int line ) {
	if (!parser.had_error) {
		parser.had_error = 1;
		parser.errors = vec_of(struct ParseError*, 8);
	}
	struct ParseError *err = malloc(sizeof(struct ParseError));
	err->type = expected;
	err->got = got;
	err->debug_file = file;
	err->debug_line = line;
	err->is_tok = is_tok;
	vec_push(parser.errors, err);
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
			parser.tokens->token_list[error->got].line,
			parser.tokens->token_list[error->got].chr_index,
			token_to_str(error->type),
			(int)parser.tokens->token_list[error->got].source.len,
			parser.tokens->token_list[error->got].source.c_ptr,
			token_to_str(parser.tokens->token_list[error->got].type));
	} else {
		printf("%s:%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
			parser.source->path,
			parser.tokens->token_list[error->got].line,
			parser.tokens->token_list[error->got].chr_index,
			token_to_str(error->type),
			(int)parser.tokens->token_list[error->got].source.len,
			parser.tokens->token_list[error->got].source.c_ptr,
			token_to_str(parser.tokens->token_list[error->got].type));
	}
}

bool check_errors(void) {
	if (parser.had_error) {
		for (int i = 0; i < parser.errors->current; i++) {
			report_error((struct ParseError*)parser.errors->data[i]);
		}
		return True;
	} else return False;
}

int match_type_token(void) {
	TokenType next = peek().type;
	if(check_type_token()) {
		consume();
		return next;
	}
	return 0;
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
	node->children = vec_of(struct Node*, 8);

	while(!match(T_EOF)) {
		struct Node* item = top_level();
		if(item != NULL) {
			if(item->tag == N_NODE_LIST) {
				for(int i = 0; i < item->children->current; i++) {
					vec_push(node->children, item->children->data[i]);
				}
			}
			else vec_push(node->children, item);
			// print_ast(*node);
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

	if (match(T_CONST)) {
		return const_def();
	}

	// should redo my function / variable parsing strategy here.
	// i should first try to match on keywords that i know for sure cannot be followed by one or the other,
	// and if i don't fine said keywords, i should then search for the `TYPE ID ( '=' / '=>' )` pattern.
	// for instance, the 'pub' keyword is guaranteed to never be a variable definition, it'll always be either a function, struct, enum, union, interface or type.
	// similarly, the `let` keyword will never ever be followed up with anything other than a variable declaration.

	// check for functions or variables. These require a lookahead to differenciate between them.
	if ( match(T_LET) ) {
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
	printf("unknown node type: %s\n", token_to_str(current_tok().type));
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
	imp->path.root = current_tok().source;
	imp->path.full = current_tok().source;

	// check for subpaths
	if(check_next(T_DOT)) {
		imp->path.has_subpaths = 1;

		while(match(T_DOT)) {
			if(!match(T_IDENTIFIER)) {
				PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
				return NULL;
			}
			imp->path.subpath[imp->path.cur_subpath] = current_tok().source; 
			imp->path.full.len += current_tok().source.len + ( current_tok().source.c_ptr - (imp->path.full.c_ptr + imp->path.full.len) );
			imp->path.cur_subpath++;
		}
	}

	if(match(T_AS)) {
		if(!match(T_IDENTIFIER)) {
			PANIC(T_IDENTIFIER, P_TOK, T_SEMI, P_TOK);
			return NULL;
		}
		imp->has_name = 1;
		imp->name_id = current_tok().literal_id;
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
	node->type = current_tok().type;
	match(T_IDENTIFIER);
	node->name_id = current_tok().literal_id;
	match(T_EQUAL_SIGN);
	match_cat(TC_LITERAL);
	node->value = current_tok().literal_id;
	match(T_SEMI);
	return node;
}

// static ^Node var_assign => {}
// fun int add (int x, y) = x + y;
// fun ^Node var_assign = {}
// let ^Node node = new Node();
// ^Node node = new Node();
// let node = new Node();

static struct Node *var_assign(void) {
	struct Node* node = new_node(N_VAR);

	if (current_tok().type == T_LET) {

		if(!match(T_IDENTIFIER)) PANIC(T_IDENTIFIER, P_TOK, TC_KEYWORD, P_CAT);
		node->name_id = current_tok().literal_id;	
	} else {

		// pull out type
		consume();

		// the type can either be a set of built-in types,
		// or an identifier that corresponds to a user defined type
		// ( which can be one of a struct, an enum, a union, or a type alias ) 
		// 

		TokenType t = current_tok().type;
		if( t == T_IDENTIFIER )
			node->type_id = current_tok().literal_id;
		node->type = current_tok().type;

		// pull out identifier
		consume();
		node->name_id = current_tok().literal_id;
	}


  if (!match(T_EQUAL_SIGN))
    PANIC(T_EQUAL_SIGN, P_TOK, TC_KEYWORD, P_CAT);

	if( !match_cat(TC_LITERAL)) 
		PANIC(TC_LITERAL, P_CAT, TC_KEYWORD, P_CAT);
	node->value = current_tok().literal_id;
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
	item->name_id = current_tok().literal_id;
	if(match(T_EQUAL_SIGN)) {
		match_cat(TC_LITERAL);
		item->value = current_tok().literal_id;
	}
	return item;
}

static struct Node* struct_field(void) {
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
	node->name_id = current_tok().literal_id;
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
	fn->type = current_tok().type;

	// match identifier
	consume();
	fn->name_id = current_tok().literal_id;

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
	struct Node* node = malloc(sizeof(struct Node));
	if (node == NULL) {
		printf("Error, could not malloc new node\n");
		exit(-1);
	}
	node->tag = type;
	node->name_id = -1;
	node->value = -1;
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


// TODO: Probably a waste of effort, but consider writing a set of proper string builder functions for generating debug parser output.
void print_lit(Literal *lit) {
	if (lit->type >= 0)
		switch (lit->type) {
			case LIT_NONE: printf("<none>"); break;
			case LIT_FALSE: printf("bool: <false>"); break;
			case LIT_TRUE: printf("bool: <true>"); break;
			case LIT_CHAR: printf("char: '%c'", lit->value.character); break;
			case LIT_INT: printf("int: %li", lit->value.integer); break;
			case LIT_FLOAT: printf("float: %lf", lit->value.floating);
			case LIT_STRING: printf("string: \"%.*s\"", (int)lit->value.string.len, lit->value.string.c_ptr);
		}
	else printf("<Uninitialized>\n");
}

void print_ast(struct Node *node, int level)
{
	if (!node) exit(-1);
	if (level == 0) printf("<root>\n");
	for ( int i = 0; i < node->children->current; i++ ) {
		printf("\t");
		struct Node *cur = node->children->data[i];
		if (cur == NULL) continue;
		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_VAR: {
					printf("<var> name: %.*s, type: <TYPE>, value: ",
						(int)lit_id(cur->name_id)->value.string.len,
						lit_id(cur->name_id)->value.string.c_ptr
					);
					print_lit(lit_id(cur->value));
					printf("\n");
				}
			break;
			case N_STRUCT_FIELD: {
				printf("<field> name: %.*s, type: %s, value: ",

					(int)lit_id(cur->name_id)->value.string.len,
					lit_id(cur->name_id)->value.string.c_ptr,
					token_to_str(cur->type)
				);
				print_lit(lit_id(cur->value));
				printf("\n");
				break;
			} 				
			case N_STRUCT_DEF: {
				printf("\n\t<STRUCT DEF>\n\t\tname: %.*s, body:\n", 

					(int)lit_id(cur->name_id)->value.string.len,
					lit_id(cur->name_id)->value.string.c_ptr
				);
				print_ast(cur, 1);
				printf("\n");
				break;
			}
			case N_FUNC_DEF: {
				printf("<fn def> name: %.*s, return: %s, body: ",
						(int)lit_id(cur->name_id)->value.string.len,
						lit_id(cur->name_id)->value.string.c_ptr,
						token_to_str(cur->type)
						);
				// printf("\t");
				print_expr(cur->body);
				printf("\n");
			}
			break;
			case N_CONST: {
					printf("<const> name: %.*s, type: %s, body:",
						(int)lit_id(cur->name_id)->value.string.len,
						lit_id(cur->name_id)->value.string.c_ptr,
						token_to_str(cur->type)
					);
				print_lit(lit_id(cur->value));
				printf("\n");
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
						(int)lit_id(cur->name_id)->value.string.len,
						lit_id(cur->name_id)->value.string.c_ptr);
				printf("\n");
				break;
			}
		}
		// printf("\n");
	}
}

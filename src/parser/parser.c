#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
// #include "memory.h"
#include "parser-internal.h"

#include "expr.h"
#include "../scanner.h"

struct Parser parser;

// #define PANIC(e, et, u, ut) panic(e, et, u, ut, __FILE__, __LINE__)

// Forward declarations so that i can rearrange the definitions as i wish
static Node *top_level  (void);

static Node *type_decl  (bool pub, bool is_alias);
static Node *import_decl(void);
static Node *const_decl (bool pub);
static Node *fn_decl    (bool pub);
static Node *struct_decl(bool pub);
static Node *enum_decl  (bool pub);
static Node *union_decl (bool pub);
// global variables are never public?
static Node *var_decl   (void);
static bool parse_params(dbuf_Param * params);

#define FAIL(EXP, MSG, RET)                                                    \
  if (expect(EXP, MSG).type == T_ERROR) {                                      \
    return RET;                                                                \
  }

#define FAIL_TOK(TOK, EXP, MSG, RET)                                           \
  Token TOK = expect(EXP, MSG);                                                \
  if (TOK.type == T_ERROR) {                                                   \
    return RET;                                                                \
  }

void init_parser( dstr source, dstr file_name, d_arena arena ) {
	parser.source = source;
	parser.cur = TOKEN_NONE;
	parser.prev = TOKEN_NONE;
	parser.had_error = false;
	parser.file_name = file_name;
	parser.arena = arena;
	parser.errors = dbuf_ParseErrors_make(8);
	parser.had_error = false;
	parser.panic_mode = false;
}

Node * new_node(NodeTag type) {
	Node * node = darena_alloc(&parser.arena, sizeof(Node));
	node->tag = type;
	node->type = (Datatype){0};
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
		case N_PROG:
		case N_NODE_LIST:
		case N_STRUCT_DEF:
		case N_BLOCK:
			node->children = dbuf_NodeList_make(8);
	}
	return node;
}

Node * parse_tokens( dstr src, dstr file_name, d_arena arena )  { 
	// Initialize parser state, parser struct found in src/parser/parser-internal.h
	scanner_init(src);
	init_parser(src, file_name, arena);
	consume(); // prime token

	Node * n = new_node(N_PROG);

	while(!match(T_EOF)) {
    const char *start = parser.cur.span.cptr;

		struct Node* item = top_level();
		if(item != NULL) {
			dbuf_NodeList_push(&n->children, item);
		}

		if(parser.panic_mode) {
			if (parser.cur.span.cptr == start) {
				consume();
			}
      synchronize();
		}
	}

	if(parser.had_error) {
		report_errors();
		return NULL;
	}
	return n;
}

static Datatype * parse_type(void) {
	Datatype * t = darena_alloc(&parser.arena, sizeof(Datatype));

	// FIXME: check for NULL in all instances of `parse_type()` here;
	if(match('*')) {
		t->tag = DATATYPE_POINTER;
		int indir = 1;
		while(match('*')) { indir++; } 
		t->type.dt_ptr.indir = indir;
		t->type.dt_ptr.to = parse_type();
	}
	else if(match('[')) {
		Token open_delim = prev_tok();
		dassert(open_delim.type=='[', "");
		// slice
		if(match('*')) {
			t->tag = DATATYPE_SLICE;
			t->type.dt_slice.size = -1;
			FAIL(']', "Unclosed delimiter", NULL)
			t->type.dt_slice.of = parse_type();
		}
		// array
		else if(match(T_LIT_INT) || match('_')) {
			t->tag = DATATYPE_ARRAY;
			if(prev_tok().type == '_') { t->type.dt_arr.size = -1; }
			else { t->type.dt_arr.size = prev_tok().value._int; }
			FAIL(']', "Unclosed delimiter", NULL)
			t->type.dt_arr.of = parse_type();
		}
		else {
			error("Expected expression");
			return NULL;
		}
	}
	// function type
	else if(match(T_FUN)) {
		t->tag = DATATYPE_FUNC;
		expect('(', "Missing parameters list");
		if(!match(')')) {
			t->type.dt_func.params = dbuf_Param_make(8);
			if(!parse_params(&t->type.dt_func.params)) {
				return NULL;
			}
			FAIL(')', "Unclosed delimiter", NULL);
		}
		if(match(':')) {
			t->type.dt_func.return_type = parse_type();
		}
	}
	// plain type
	else if (match(T_IDEN)) {
		t->tag = DATATYPE_IDENTIFIER;
		t->type.dt_iden = prev_tok().span;
	}
	else {
		error("Malformed type");
		return NULL;
	}

	return t;
}

static bool param_group(dbuf_Param* params) {
	isize first = params->cur;

	do {
		Token name = expect(T_IDEN, "Missing param name");
		if(name.type == T_ERROR) return false;

		dbuf_Param_push(params, (Param){.iden = name.span});
	} while(match(','));

	FAIL(':', "Expected return type", false);

	Datatype * type = parse_type();
	if(!type) {
		return false;
	}

	Node * def = NULL;
	if(match('=')) {
		Token eq = prev_tok();

		def = parse_expr();
		if(!def) {
			return false;
		}

		if(params->cur - first > 1) {
			error_at(eq, "Default values not allowed on grouped parameters, try splitting them instead");
			return false;
		}

		params->data[first].def_value = def;
	}

	for(isize i = first; i < params->cur; i++) {
		params->data[i].type = type;
	}

	return true;
}

static bool parse_params(dbuf_Param* params) {
	do {
		if(!param_group(params)) {
			return false;
		}
	} while(match(','));
	return true;
}

static Node* import_decl(void) {
	Node * n = new_node(N_IMPORT);

	FAIL_TOK(iden, T_IDEN, "Missing import name", NULL);

	Path path = {0};
	path.root = iden.span;
	path.full = iden.span;

	int full_len = iden.span.len;
	while (match('.')) {
		FAIL_TOK(subpath, T_IDEN, "Missing subpath name", NULL);

		path.has_subpaths = true;
		path.subpath[path.cur_subpath] = subpath.span;
		// FIXME: Add bounds checking, since you can only have 32 subpaths.
		path.cur_subpath++;
		full_len += subpath.span.len;
	}

	if(path.has_subpaths) {
		path.full.len = full_len + path.cur_subpath;
		char * buf = darena_alloc(&parser.arena, sizeof(char)*(path.full.len+1));

		char * p = buf;
		memcpy(p, iden.span.cptr, iden.span.len);
		p += iden.span.len;

		for(int i = 0; i < path.cur_subpath; i++) {
			*p++ = '.';
			memcpy(p, path.subpath[i].cptr, path.subpath[i].len);
			p += path.subpath[i].len;
		}
		*p = '\0';
		path.full.cptr = buf;
	}

	n->path = path;

	if (match(T_AS)) {
		n->iden = expect(T_IDEN, "Missing alias").span;
	} else {
		// set the final subpath item as the module identifier
		// i.e import sys.math.vector; sets vector as the module name
		// vs import io; sets io as a the module name
		n->iden = path.subpath[path.cur_subpath - 1];
	}

	if(expect(';', "Missing ';' after import").type == T_ERROR) {
		return NULL;
	}
	return n;
}


static Node* const_decl(bool public) {
	Token iden = expect(T_IDEN, "Missing identifier");
	Datatype * ty = NULL;

	FAIL(':', "Missing type", NULL);
	ty = parse_type();
	if(!ty) return NULL;

	Token ass = expect(T_ASSIGN, "Missing :=");
	if (iden.type == T_ERROR || ass.type == T_ERROR) {
		return NULL;
	}

	// FIXME: Temp hack until i add proper expression parshing later
	while(!match(';')) consume();
	// Node * rhs = parse_expr();

	Node * n = new_node(N_CONST);
	n->iden = iden.span;
	n->type = *ty;
	// n->rhs = rhs;	
	return n;
}

static Node* var_decl(void) { return new_node(N_TODO); }
static Node* type_decl(bool pub, bool is_alias) { return new_node(N_TODO); }
static Node *fn_decl(bool public) { dassert(false, "TODO"); }

static Node * top_level(void) {
	bool pub = false;
	if(match(T_PUB)) {
		pub = true;
	}

	if(match(T_ALIAS)) return type_decl(pub, true);
	if(match(T_CONST)) return const_decl(pub);
	if(match(T_FUN)) return fn_decl(pub);
	if(match(T_IMPORT)) return import_decl();
	if(match(T_LET)) return var_decl();
	if(match(T_TYPE)) return type_decl(pub, false);

	// skip unknown characters for now *shrug*
	// maybe panic here?
	// dunno
	// printf("unknown node type: %s\n", token_to_str(current_tok().type));
	error_at(cur_tok(), "Invalid token idk");
	return NULL;
}

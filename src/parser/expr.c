#include "parser.h"
#include "expr.h"
#include "../scanner.h"

#include <stdio.h>

static Node *term(void);
static Node *factor(void);
// static struct Node *unary(void);
static Node *primary(void);

static Node *new_un_expr(TokenTag type, struct Node *rhs);
static Node *new_bin_expr(TokenTag type, struct Node *rhs, struct Node *lhs);


static UnOpType token_to_unop(TokenTag type);
static BinOpType token_to_binop(TokenTag type);

static const char* binop_to_string(BinOpType type);
static const char* unop_to_string( UnOpType type);

// static struct Node *assignment(void) {
// 	if(check_next(T_IDENTIFIER) && check_next_n(2, T_EQUAL_SIGN)) {
		
// 	}
// 	match(T_EQUAL_SIGN);
// 	match(T_INTEGER_LITERAL);
// }

Node *parse_expr(void) {

	struct Node *expr = term();

	while (match(T_PLUS) || match(T_MINUS)) {
		TokenTag op = current_tok().type;
		struct Node *rhs = term();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

static Node *term(void) {
	struct Node *expr = factor();

	while (match(T_STAR) || match(T_SLASH)) {
		TokenTag op = current_tok().type;
		struct Node *rhs = factor();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

static Node *factor(void) {
	if (match(T_BANG) || match(T_MINUS)) {
		TokenTag op = current_tok().type;
		struct Node *right = factor();
		struct Node *expr = new_un_expr(op, right);
		return expr;
	}
	return primary();
}

static Node *primary(void) {
	if (match(T_INTEGER_LITERAL)){
		struct Node *node = new_node(N_LIT);
		node->value = cur_tok_span();
		return node;
	}

	// if (match(T_DOUBLE_FLOATING_LITERAL)){
	// 	struct Node *node = new_node(N_LIT);
	// 	node->value = lit_at(current())->literal._double;
	// 	return node;
	// }
	//
	if (match(T_STRING_LITERAL)){
		struct Node *node = new_node(N_LIT);
		node->value = cur_tok_span();
		return node;
	}
	//
	// if (match(T_TRUE)){
	// 	struct Node *node = new_node(N_LIT);
	// 	node->value = lit_at(current()).literal._int;
	// 	return node;
	// }
	//
	// if (match(T_FALSE)){
	// 	struct Node *node = new_node(N_LIT);
	// 	node->value = lit_at(current()).literal._int;
	// 	return node;
	// }
	return NULL;
}


static Node *new_bin_expr(TokenTag type, struct Node *lhs, struct Node *rhs) {
	struct Node *expr = new_node(N_BIN_EXPR);
	expr->lhs = lhs;
	expr->rhs = rhs;
	expr->op = type;
	return expr;
}


static Node *new_un_expr( TokenTag type, Node *rhs) {
	struct Node *expr = new_node(N_UN_EXPR);
	expr->rhs = rhs;
	expr->op = type;
	return expr;
}

static UnOpType token_to_unop(TokenTag type) {
	switch (type) {
		case T_MINUS: return UN_MINUS; break;
		case T_AMP: return UN_ADDR; break;
		case T_BANG: return UN_BANG; break;
		case T_STAR: return UN_DEREF; break;
		default: return UN_NONE; break;
	}
}

static BinOpType token_to_binop(TokenTag type) {
	switch(type) {
		case T_PLUS: return BIN_ADD; break;
		case T_MINUS: return BIN_SUB; break;
		case T_STAR: return BIN_MULT; break;
		case T_SLASH: return BIN_DIV; break;
		case T_MODULUS: return BIN_MOD; break;
		case T_AMP: return BIN_BAND; break;
		case T_PIPE: return BIN_BOR; break;
		case T_HAT: return BIN_BXOR; break;
		default: return BIN_NONE; break;
	}
}

static const char* binop_to_string(BinOpType type) {
	switch(type) {
		case BIN_NONE: return "none!!"; break;
		case BIN_ADD: return "+"; break;
		case BIN_SUB: return "-"; break;
		case BIN_MULT: return "*"; break;
		case BIN_DIV: return "/"; break;
		case BIN_MOD: return "%"; break;
		case BIN_BAND: return "&"; break;
		case BIN_BOR: return "|"; break;
		case BIN_BXOR: return "^"; break;
	}
}

static const char* unop_to_string( UnOpType type ) {
	switch(type) {
		case UN_NONE: return "0"; break;
		case UN_MINUS: return "-"; break;
		case UN_ADDR: return "&"; break;
		case UN_BANG: return "!"; break;
		case UN_DEREF: return "*"; break;
	}
}

void print_expr( Node *expr ) {
	switch(expr->tag) {
		case N_BIN_EXPR: {
				printf("(");
				print_expr(expr->lhs);
				printf(" %s ", tok_to_str(expr->op));
				print_expr(expr->rhs);
				printf(")");
			break;
		}
		case N_UN_EXPR:
		{
			printf("%s(", tok_to_str(expr->op));
			print_expr(expr->rhs);
			printf(")");
		break;
		}
		case N_LIT:
		{
			printf(" %.*s ", 
			  expr->value->len,
			  expr->value->c_ptr
	     );
			break;
		}
		default:
		break;
	}
}

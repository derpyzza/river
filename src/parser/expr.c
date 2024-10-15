#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

#include <stdio.h>

static struct node *un_expr(void);
static struct node *bin_expr(void);

static struct node *term(void);
static struct node *factor(void);
static struct node *unary(void);
static struct node *primary(void);

static struct node *new_un_expr(enum UnOpType type, struct node *rhs);
static struct node *new_bin_expr(enum BinOpType type, struct node *rhs, struct node *lhs);


static enum UnOpType token_to_unop(token_type type);
static enum BinOpType token_to_binop(token_type type);

struct node *parse_expr(void) {

	struct node *expr = term();

	while (match(T_PLUS) || match(T_MINUS)) {
		int op = token_to_binop(current_tok().type);
		struct node *rhs = term();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

static struct node *term(void) {
	struct node *expr = factor();

	while (match(T_ASTERISK) || match(T_FORWARD_SLASH)) {
		int op = token_to_binop(current_tok().type);
		struct node *rhs = factor();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

static struct node *factor(void) {
	if (match(T_BANG) || match(T_MINUS)) {
		int op = token_to_unop(current_tok().type);
		struct node *right = factor();
		struct node *expr = new_un_expr(op, right);
		return expr;
	}
	return primary();
}

static struct node *primary(void) {
	if (match(T_INTEGER_LITERAL)){
		struct node *node = new_node(N_LIT);
		node->value = lit_at(current())->literal._int;
		return node;
	}

	// if (match(T_DOUBLE_FLOATING_LITERAL)){
	// 	struct node *node = new_node(N_LIT);
	// 	node->value = lit_at(current())->literal._double;
	// 	return node;
	// }
	//
	// if (match(T_STRING_LITERAL)){
	// 	struct node *node = new_node(N_LIT);
	// 	node->value = lit_at(current()).literal._int;
	// 	return node;
	// }
	//
	// if (match(T_TRUE)){
	// 	struct node *node = new_node(N_LIT);
	// 	node->value = lit_at(current()).literal._int;
	// 	return node;
	// }
	//
	// if (match(T_FALSE)){
	// 	struct node *node = new_node(N_LIT);
	// 	node->value = lit_at(current()).literal._int;
	// 	return node;
	// }
	return NULL;
}


static struct node *new_bin_expr(enum BinOpType type, struct node *lhs, struct node *rhs) {
	struct node *expr = new_node(N_BIN_EXPR);
	expr->lhs = lhs;
	expr->rhs = rhs;
	expr->type = type;
	return expr;
}


static struct node *new_un_expr(enum UnOpType type, struct node *rhs) {
	struct node *expr = new_node(N_UN_EXPR);
	expr->rhs = rhs;
	expr->type = type;
	return expr;
}

static enum UnOpType token_to_unop(token_type type) {
	switch (type) {
		case T_MINUS: return UN_MINUS; break;
		case T_AMP: return UN_ADDR; break;
		case T_BANG: return UN_BANG; break;
		case T_ASTERISK: return UN_DEREF; break;
		default: return UN_NONE; break;
	}
}

static enum BinOpType token_to_binop(token_type type) {
	switch(type) {
		case T_PLUS: return BIN_ADD; break;
		case T_MINUS: return BIN_SUB; break;
		case T_ASTERISK: return BIN_MULT; break;
		case T_FORWARD_SLASH: return BIN_DIV; break;
		case T_MODULUS: return BIN_MOD; break;
		case T_AMP: return BIN_BAND; break;
		case T_PIPE: return BIN_BOR; break;
		case T_HAT: return BIN_BXOR; break;
		default: return BIN_NONE; break;
	}
}

static char* bin_op_to_string(enum BinOpType type) {
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

static char un_op_to_str(enum UnOpType type) {
	switch(type) {
		case UN_NONE: return '0'; break;
		case UN_MINUS: return '-'; break;
		case UN_ADDR: return '&'; break;
		case UN_BANG: return '!'; break;
		case UN_DEREF: return '*'; break;
	}
}

void print_expr(struct node *expr) {
	switch(expr->tag) {
		case N_BIN_EXPR: {
				printf("(binexpr: ");
				print_expr(expr->lhs);
				printf(" op: %s ", bin_op_to_string(expr->type));
				print_expr(expr->rhs);
				printf(");");
			break;
		}
		case N_UN_EXPR:
		{
			printf("unexpr: %c(", un_op_to_str(expr->type));
			print_expr(expr->rhs);
			printf(")");
		break;
		}
		case N_LIT:
		{
			printf(" lit: %lli ", expr->value);
			break;
		}
		default:
		break;
	}
}

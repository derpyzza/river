#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

static struct NodeExpr *expr(void);
static struct NodeUnExpr *un_expr(void);
static struct NodeBinExpr *bin_expr(void);

static struct NodeExpr *term(void);
static struct NodeExpr *factor(void);
static struct NodeExpr *unary(void);
static struct NodeExpr *primary(void);

static struct NodeExpr *new_expr(int type);

static struct NodeExpr *expr(void) {

	NodeExpr *expr = term();

	while (match(T_PLUS) || match(T_MINUS)) {
		int op = current_tok().type;
		// printf("expr: %s, %c, %i\n", token_to_str(op), token_to_char(op), op);
		NodeExpr *rhs = term();
		expr->type = N_EXP_BIN;
		expr->expr.bin = malloc(sizeof(NodeBinExpr));
		expr->expr.bin->lhs = expr;
		expr->expr.bin->rhs = rhs;
		expr->expr.bin->op_type = op;
	}
	return expr;
}

 static struct NodeExpr *term(void) {
	NodeExpr  *expr = factor();

	while (match(T_ASTERISK) || match(T_FORWARD_SLASH)) {
		int op = current_tok().type;
		// printf("term: %s, %c\n", token_to_str(op), token_to_char(op));
		NodeExpr *rhs = factor();
		expr->type = N_EXP_BIN;
		expr->expr.bin = malloc(sizeof(NodeBinExpr));
		expr->expr.bin->lhs = expr;
		expr->expr.bin->rhs = rhs;
		expr->expr.bin->op_type = op;
	}
	return expr;
}

static struct NodeExpr *factor(void) {

	if (match(T_BANG) || match(T_MINUS) || match(T_ASTERISK) || match(T_AMP)) {
		int op = current_tok().type;
		NodeExpr *right = factor();
		NodeExpr *expr = malloc(sizeof(NodeExpr));
		expr->type = N_EXP_UN;
		expr->expr.un = malloc(sizeof(NodeUnExpr));
		expr->expr.un->operand = right;
		expr->expr.un->op_type = op;
		return expr;
	}
	return primary();
}

static struct NodeExpr *primary(void) {
	return &(NodeExpr) {0};
	if (match(T_INTEGER_LITERAL)){
		NodeExpr *node = new_expr(N_LIT_INT);
		node->expr.lit = lit_at(current()).literal._int;
		return node;
	}
	//
	// if (match(T_DOUBLE_FLOATING_LITERAL)){
	// 	NodeExpr *node = new_expr(N_LIT_DOUBLE);
	// 	node->expr.n_double = lit_at(current()).literal._double;
	// 	return node;
	// }
	//
	// if (match(T_STRING_LITERAL)){
	// 	NodeExpr *node = new_expr(N_LIT_STRING);
	// 	node->expr.n_string = lit_at(current()).literal._str;
	// 	return node;
	// }
	//
	// if (match(T_TRUE)){
	// 	NodeExpr *node = new_expr(E_LIT_TRUE);
	// 	return node;
	// }
	//
	// if (match(T_FALSE)){
	// 	NodeExpr *node = new_expr(E_LIT_FALSE);
	// 	return node;
	// }
}

static struct NodeExpr *new_expr(int type) {
	NodeExpr* node = malloc(sizeof(NodeExpr));
	if (node == NULL) {
		printf("ERROR: couldn't malloc a new node\n");
		return NULL;
	}
	node->type = type;
	return node;
}

char* bin_op_to_string(enum BinOpType type) {
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

void print_expr(NodeExpr *expr) {
	switch(expr->type) {
		case E_BIN_EXPR:
			printf("binexpr: ");
			print_expr(expr->expr.bin->lhs);
			printf("op: %s", bin_op_to_string(expr->expr.bin->op_type));
			print_expr(expr->expr.bin->rhs);
			printf(";\n");
		break;
		case E_UN_EXPR:
		break;
		case E_LIT:
			printf("lit: %i", expr->expr.lit);
		break;
		default:
		break;
	}
}

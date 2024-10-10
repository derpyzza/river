#include "parser.h"
#include "parser_utils.h"
#include "expr.h"

static struct NodeUnExpr *un_expr(void);
static struct NodeBinExpr *bin_expr(void);

static struct NodeExpr *term(void);
static struct NodeExpr *factor(void);
static struct NodeExpr *unary(void);
static struct NodeExpr *primary(void);

static struct NodeExpr *new_expr(int type);
static struct NodeExpr *new_un_expr(enum UnOpType type, NodeExpr *rhs);
static struct NodeExpr *new_bin_expr(enum BinOpType type, NodeExpr *rhs, NodeExpr *lhs);


static enum UnOpType token_to_unop(token_type type);
static enum BinOpType token_to_binop(token_type type);

struct NodeExpr *parse_expr(void) {

	NodeExpr *expr = term();

	while (match(T_PLUS) || match(T_MINUS)) {
		int op = token_to_binop(current_tok().type);
		NodeExpr *rhs = term();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

struct NodeBlockExpr *block_expr(void) {
	return NULL;
}

 static struct NodeExpr *term(void) {
	NodeExpr  *expr = factor();

	while (match(T_ASTERISK) || match(T_FORWARD_SLASH)) {
		int op = token_to_binop(current_tok().type);
		NodeExpr *rhs = factor();
		expr = new_bin_expr(op, expr, rhs);
	}
	return expr;
}

static struct NodeExpr *factor(void) {
	if (match(T_BANG) || match(T_MINUS)) {
		int op = token_to_unop(current_tok().type);
		NodeExpr *right = factor();
		NodeExpr *expr = new_un_expr(op, right);
		return expr;
	}
	return primary();
}

static struct NodeExpr *primary(void) {
	if (match(T_INTEGER_LITERAL)){
		NodeExpr *node = new_expr(E_LIT);
		node->lit = lit_at(current()).literal._int;
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
	return NULL;
}


static struct NodeExpr *new_expr(int type) {
	NodeExpr* node = malloc(sizeof(NodeExpr));
	if (node == NULL) {
		printf("ERROR: couldn't malloc a new expr node\n");
		return NULL;
	}
	node->type = type;
	return node;
}


static struct NodeExpr *new_bin_expr(enum BinOpType type, NodeExpr *lhs, NodeExpr *rhs) {
	NodeExpr *expr = new_expr(E_BIN_EXPR);
	expr->bin = malloc(sizeof(NodeBinExpr));
	expr->bin->lhs = lhs;
	expr->bin->rhs = rhs;
	expr->bin->op_type = type;
	return expr;
}


static struct NodeExpr *new_un_expr(enum UnOpType type, NodeExpr *rhs) {
	NodeExpr *expr = new_expr(E_UN_EXPR);
	expr->un = malloc(sizeof(NodeUnExpr));
	expr->un->operand = rhs;
	expr->un->op_type = type;
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

void print_expr(NodeExpr *expr) {
	switch(expr->type) {
		case E_BIN_EXPR: {
				printf("(binexpr: ");
				print_expr(expr->bin->lhs);
				printf(" op: %s ", bin_op_to_string(expr->bin->op_type));
				print_expr(expr->bin->rhs);
				printf(");");
			break;
		}
		case E_UN_EXPR:
		{
			printf("unexpr: %c(", un_op_to_str(expr->un->op_type));
			print_expr(expr->un->operand);
			printf(")");
		break;
		}
		case E_LIT:
		{
			printf(" lit: %i ", expr->lit);
			break;
		}
		default:
		break;
	}
}

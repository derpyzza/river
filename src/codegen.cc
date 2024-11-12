#include "utils.h"
#include "codegen.h"
#include "parser/parser.h"
#include "scanner.h"
#include <bits/posix2_lim.h>

static char* function(Node *tree);
static char* gen_expr(Node *expr);

void codegen(FILE* file, Node *tree) {
 	switch(tree->tag) {
 		case N_FUNC_DEF:
 			fprintf(file, "%s", function(tree));
 		break;
 		default:
    break;
 	}
	fprintf(file, "\n");
 }


 static char* function(Node *tree) {
 	char* str = malloc(sizeof(char) * 128);
 	// char* body = malloc(sizeof(char) * 128);
 	String id = lit_id(tree->name_id)->value.string;
 	sprintf(str, "%s %.*s(void) {\n return %s; \n}\n",
 			token_to_str(tree->type),
 			id.len,
 			id.c_ptr,
 			gen_expr(tree->body)
 			);
 	return str;
}

static char* gen_expr(Node *expr) {
  String *s = str_new(1024);

  switch(expr->tag) {
    case N_BIN_EXPR: {
      sprintf(s->c_ptr, "%s %s %s", gen_expr(expr->lhs), expr->op, gen_expr(expr->rhs));
      break;
    }
    case N_UN_EXPR: {
      sprintf(s->c_ptr, "%s %s", expr->op, gen_expr(expr->rhs));
      break;
    }

    case N_LIT: {  
        sprintf(s->c_ptr, "%li", lit_id(expr->value)->value.integer);
    }
    default:
    break;
  }
  return s->c_ptr;
}

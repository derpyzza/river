#include "utils.h"
#include "codegen.h"
#include "parser/parser.h"
#include "scanner.h"
// #include "scanner.h"

// static char* function(Node *tree);
// static char* gen_expr(Node *expr);

void codegen(FILE* file, Node *tree) {
  NULL_PTR_PANIC(tree);
 	switch(tree->tag) {
      case N_BLOCK:
        fprintf(file, "{\n");
        for (int i = 0; i < tree->children->current; i++) {
          codegen(file, tree->children->data[i]);
        }
        fprintf(file, "\n}\n");
      break;
 		case N_FUNC_DEF:
 			fprintf(file, "%.*s %.*s () {\n\treturn ",
 			  tree->type->len,
 			  tree->type->c_ptr,
 			  tree->name->len,
 			  tree->name->c_ptr
 			);
      codegen(file, tree->body);
      fprintf(file, ";\n}\n");
 		break;
 		case N_BIN_EXPR:
 		  codegen(file, tree->lhs);
 		  fprintf(file, "%s", tok_to_str(tree->op));
 		  codegen(file, tree->rhs);
 		break;
 		case N_UN_EXPR:
 		  fprintf(file, "%s", tok_to_str(tree->op));
      codegen(file, tree->rhs);
 		break;
    case N_LIT:
      fprintf(file, "%.*s", tree->value->len, tree->value->c_ptr);
    break;
 		default:
 		  fprintf(file, "// DERP\n");
    break;
 	}
	// fprintf(file, "\n");
 }


//  static char* function(Node *tree) {
//  	char* str = malloc(sizeof(char) * 128);
//  	// char* body = malloc(sizeof(char) * 128);
//  	String id = lit_id(tree->name_id)->value.string;
//  	sprintf(str, "%s %.*s(void) {\n return %s; \n}\n",
//  			token_to_str(tree->type),
//  			id.len,
//  			id.c_ptr,
//  			gen_expr(tree->body)
//  			);
//  	return str;
// }

// static char* gen_expr(Node *expr) {
//   String *s = str_new(1024);

//   switch(expr->tag) {
//     case N_BIN_EXPR: {
//       sprintf(s->c_ptr, "%s %s %s", gen_expr(expr->lhs), expr->op, gen_expr(expr->rhs));
//       break;
//     }
//     case N_UN_EXPR: {
//       sprintf(s->c_ptr, "%s %s", expr->op, gen_expr(expr->rhs));
//       break;
//     }

//     case N_LIT: {  
//         sprintf(s->c_ptr, "%li", lit_id(expr->value)->value.integer);
//     }
//     default:
//     break;
//   }
//   return s->c_ptr;
// }

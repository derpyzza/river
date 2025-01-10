#include "parser.h"
#include "expr.h"


// TODO: Probably a waste of effort, but consider writing a set of proper string builder functions for generating debug parser output.
void print_ast( Node *node, int level ) {
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

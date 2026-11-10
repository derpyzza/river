#include "derp.h"
#include "parser.h"
#include "expr.h"

const char* node_to_str(NodeTag node) {
	switch(node) {
		#define NODE(N,S) case N: return S;
		NODES
		#undef NODE
		default:
			return "UNKNOWN NODE";
	}
}

void ind(int d) { for(int i = 0; i < d; i++) printf(" "); printf("\n"); }

void print_kids( Node* n, int d) {
	for(int i = 0; i < n->children.cur; i++) {
		ind(d+1);
		print_ast(n, d+1);
	}
}

// TODO: Probably a waste of effort, but consider writing a set of proper string builder functions for generating debug parser output.
void print_ast( Node *node, int depth ) {
	if (!node) {
		dlog_error("Node is NULL, returning...");
	}

	if(!node->children.data) {
		dlog_error("Node has no children");
		return;
	}
	for ( int i = 0; i < node->children.cur; i++ ) {
		for( int i = 0; i < depth; i++) printf(" ");
		struct Node *cur = (Node*)node->children.data[i];
		if (cur == NULL) {
			dlog_warn("Node, child #%i of %s is NULL", i, node_to_str(cur->tag));
			continue;
		}

		switch(cur->tag) {
			default: printf("Unknown or not implemented yet!\n"); break;
			case N_NONE: printf("None!\n"); break;
			case N_TODO: printf("todo!!!\n"); break;
			case N_PROG: {
				printf("(prog ");
				print_kids(cur, depth);
				break;
			}

			case N_IMPORT: {
				dstr root = cur->path.root;
				printf("<imp def> path: %.*s, root: %.*s", 
					dstr_fmt(cur->path.full),
					dstr_fmt(root)
				);
				printf(", alias: %.*s",
					dstr_fmt(cur->iden)
				);
				printf("\n");
				break;
			}

	// 		case N_VAR: {
	// 			printf("<var> name: %.*s, type: <TYPE>, rhs: ",
	// 				cur->name.len,
	// 				cur->name.cptr
	// 			);
	// 			print_expr(cur->rhs);
	// 			printf("; \n");
	// 			break;
	// 		}
	// 		case N_STRUCT_FIELD: {
	// 			if (cur->rhs) {
	// 				printf("<field> name: %.*s, type: %.*s, default value: ",
	// 					cur->name.len,
	// 					cur->name.cptr,
	// 					cur->type.len,
	// 					cur->type.cptr
	// 				);
	// 				print_expr(cur->rhs);
	// 				printf("; \n");
	// 			} else 
	// 				printf("<field> name: %.*s, type: %.*s;\n",
	// 					cur->name.len,
	// 					cur->name.cptr,
	// 					cur->type.len,
	// 					cur->type.cptr
	// 				);
	// 			break;
	// 		} 				
	// 		case N_STRUCT_DEF: {
	// 			printf("\n\t<STRUCT DEF>\n\t\tname: %.*s, body:\n", 
	// 				cur->name.len,
	// 				cur->name.cptr
	// 			);
	// 			print_ast(cur, 1);
	// 			printf("\n");

	// 			break;
	// 		}
	// 		case N_FUNC_DEF: {
	// 			printf("<fn def> name: %.*s, return: %.*s, body: ",
	// 				cur->name.len,
	// 				cur->name.cptr,
	// 				cur->type.len,
	// 				cur->type.cptr
	// 			);
	// 			print_expr(cur->body);
	// 			printf("; \n");

	// 			break;
	// 		}
	// 		case N_CONST: {
	// 				printf("<const> name: %.*s, type: %*.s, body:",
	// 					cur->name.len,
	// 					cur->name.cptr,
	// 					cur->type.len,
	// 					cur->type.cptr
	// 				);
	// 			print_expr(cur->rhs);
	// 			printf(" ;\n");

	// 			break;
	// 		}
		// }
		// printf("\n");
		}
	}
}

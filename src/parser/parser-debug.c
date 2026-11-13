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

void ind(int d) { for(int i = 0; i < d; i++) printf(" "); }

void print_type( Datatype * ty, int d ) {
	switch(ty->tag) {
		case DATATYPE_IDENTIFIER:
			printf("{sym-type :name %.*s}", dstr_fmt(ty->type.dt_iden));
		break;
		case DATATYPE_POINTER:
			printf("{pointer :level %i :to ", ty->type.dt_ptr.indir);
			print_type(ty->type.dt_ptr.to, d);
			printf("}");
		break;
		case DATATYPE_ARRAY:
			printf("{array :size %i :of ");
			print_type(ty->type.dt_arr.of, d);
			printf("}");
		break;
		case DATATYPE_SLICE:
			printf("{slice :of ");
			print_type(ty->type.dt_slice.of, d);
			printf("}");
		break;
		case DATATYPE_TUPLE:
			printf("TODO");
		break;
		case DATATYPE_FUNC:
			printf("{fun :params [");
			dbuf_foreach(Param, param, ty->type.dt_func.params) {
				printf("\n");
				ind(d+1);
				printf("{param :iden %.*s :type ", dstr_fmt(param->iden));
				print_type(param->type, d);
				printf("}");
			}
			printf("] :return ");
			print_type(ty->type.dt_func.return_type, d);

		break;
	}
}

void print_kids( Node* n, int d) {
	for(int i = 0; i < n->children.cur; i++) {
		ind(d+1);
		print_ast(n->children.data[i], d+1);
	}
}

// TODO: Probably a waste of effort, but consider writing a set of proper string builder functions for generating debug parser output.
void print_ast( Node *node, int depth ) {
	if (!node) {
		dlog_error("Node is NULL, returning...");
		return;
	}

	switch(node->tag) {
		default: printf("Unknown or not implemented yet!\n"); break;
		case N_NONE: printf("None!\n"); break;
		case N_TODO: printf("todo!!!\n"); break;
		case N_PROG: {
			printf("(prog \n");
			print_kids(node, depth);
			printf(")\n");
			break;
		}

		case N_IMPORT: {
			dstr root = node->path.root;
			printf("(import :name \"%.*s\" :full_path \"%.*s\" :root \"%.*s\")\n", 
				dstr_fmt(node->iden),
				dstr_fmt(node->path.full),
				dstr_fmt(root)
			);
			break;
		}

		case N_CONST: {
			printf("(const :name \"%.*s\" :type ", dstr_fmt(node->iden));
			print_type(&node->type, depth);
			printf(")\n");
			// print_ast(node->rhs, depth+1);
			// printf("\n)");
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

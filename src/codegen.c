#include "codegen.h"
#include "parser.h"
#include "scanner.h"

static char* function(node_s *tree);

char* codegen(node_s *tree) {
	switch(tree->type) {
		case N_FN_DEF:
			return function(tree);
		break;
	}
	return "todo!";
}


static char* function(node_s *tree) {
	char* str = malloc(sizeof(char) * 128);
	char* body = malloc(sizeof(char) * 128);
	sprintf(str, "%s %.*s(void) {\n return %s; \n}\n",
			key_words[tree->node.func_def.func_sig.return_type - TKN_UBYTE],
			tree->node.func_def.func_sig.func_name.len,
			tree->node.func_def.func_sig.func_name.c_ptr,
			node_to_string(body, *tree->node.func_def.body)
			);
	return str;
}

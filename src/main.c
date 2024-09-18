#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int char_is_digit(char c) {
	return (c >= '0' && c <= '9');
}

static inline int char_is_alpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static inline int char_is_whitespace(char c) {
	return (c == ' ' || c == '\n' || c == '\t');
}

static inline char peek(char* s) {
	return (*++s);
}

typedef enum token_type {
	TOKEN_NONE,
	TOKEN_IDENTIFIER,
	TOKEN_ASSIGNMENT,
	TOKEN_NUMBER,
	TOKEN_PAREN_OPEN,
	TOKEN_PAREN_CLOSE,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MULT,
	TOKEN_DIV,
	TOKEN_SEMI,
	TOKEN_VALUE,
	TOKEN_EXPRESSION,
	TOKEN_KEYWORD,

	MAX_TOKENS
} token_type;

const char* key_words[] = {
	"int", "bool", "long", "double",
	"char", "float", "void", "if", "else",
	"for", "while", "enum", "typedef",
	"struct", "static", "inline", "return",
	"const", "switch",

	"in", "new", "delete", "namespace", "defer",
	"match"
};

const char* token_strings[] = {
	"NONE",
	"IDENTIFIER",
	"ASSIGNMENT",
	"NUMBER",
	"PAREN_OPEN",
	"PAREN_CLOSE",
	"ADD",
	"SUB",
	"MULT",
	"DIV",
	"SEMI",
	"VALUE",
	"EXPRESSION"
};

typedef struct token_s {
	token_type type;
} token_s;


typedef struct ast_s {
	long max_tokens;
	long current_token;
	token_s* token_list;
} tokenizer_s;

tokenizer_s* init_tokenizer(void) {
	tokenizer_s* tkn = malloc(sizeof(tokenizer_s));
	tkn->max_tokens = 1024;
	tkn->current_token = 0;
	tkn->token_list = malloc(sizeof(token_s) * tkn->max_tokens);
	tkn->token_list = memset(tkn->token_list, TOKEN_NONE, sizeof(token_s) * 1024);

	return tkn;
}

void push_token(tokenizer_s* ast, token_type token) {
	// Check for buffer overflows
	if (ast->current_token + 1 > ast->max_tokens) {
		ast->max_tokens += 1024;
		ast->token_list = realloc(ast->token_list, ast->max_tokens);
	}
	
	ast->token_list[ast->current_token].type = token;
	ast->current_token++;
}

char* strip_whitespace(const char* string) {
	int index = 0, str_size = strlen(string);
	int s_index = 0;
	char* s = malloc(sizeof(char) * str_size);
	while (index + 1 <= str_size ) {
		if (!char_is_whitespace(string[index])) {
			s[s_index] = string[index];
			s_index++;
			index++;
			continue;
		}
		index++;
	}
	return s;
}

void print_tokens(tokenizer_s tkn, const char* code)
{
	code = strip_whitespace(code);
	printf("===DUMPING TOKENS===\n");
	int i = 0;
	for (i = 0; i < tkn.current_token; i++ ) {
		printf("[%02i, %c] TOKEN: %s\n", i, code[i], token_strings[tkn.token_list[i].type]);
	}
	printf("Total used tokens: %i, Unused tokens: %ld\n", i, tkn.max_tokens - tkn.current_token);
}


// convert input string into AST;
tokenizer_s* parse(const char* code) {
	tokenizer_s* tkn = init_tokenizer();

	printf("== Printing code ==\n");
	for (int c = 0; code[c] != '\0'; c++) {
		if (char_is_digit(code[c])) {
			push_token(tkn, TOKEN_VALUE);
		} else if (char_is_alpha(code[c])) {
			push_token(tkn, TOKEN_IDENTIFIER);
		}
		else {
			switch (code[c]) {
				case '*':
					printf("peek: %c", peek((char*)code));
					push_token(tkn, TOKEN_MULT);
				break;
				case '+':
					push_token(tkn, TOKEN_ADD);
				break;
				case '-':
					push_token(tkn, TOKEN_SUB);
				break;
				case '/':
					push_token(tkn, TOKEN_DIV);
				break;
				case '=':
					push_token(tkn, TOKEN_ASSIGNMENT);
				break;
				case '(':
					push_token(tkn, TOKEN_PAREN_OPEN);
				break;
				case ')':
					push_token(tkn, TOKEN_PAREN_CLOSE);
				break;
				case ';':
					push_token(tkn, TOKEN_SEMI);
				break;
				case ' ':
				case '\n': 
				case '\t':
				break;
				default:
					putchar(code[c]);
				break;
			}
		}
	}
	printf("\n");

	return tkn;
}

int
main(void) {
	const char* code = "int this = 1012 * ( 2 + 4 );";
	tokenizer_s* tkn = parse(code);
	printf("stripped: %s\n", strip_whitespace(code));
	print_tokens(*tkn, code);
	free(tkn);
}

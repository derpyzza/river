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
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

static inline char peek(char* s) {
	return (*++s);
}

typedef enum token_type {
	TOKEN_NONE = 0,
	TOKEN_IDENTIFIER,
	TOKEN_EQUAL_SIGN,
	TOKEN_NUMBER,
	TOKEN_PAREN_OPEN,
	TOKEN_PAREN_CLOSE,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_ASTERISK,
	TOKEN_FORWARD_SLASH,
	TOKEN_SEMI,
	TOKEN_VALUE,
	TOKEN_EXPRESSION,
	TOKEN_KEYWORD,

	// operators
	TOKEN_DIV_EQ,
	TOKEN_MULT_EQ,
	TOKEN_ADD_EQ,
	TOKEN_SUB_EQ,
	TOKEN_NOT_EQ,
	TOKEN_EQ,
	TOKEN_LESS_THAN,
	TOKEN_LESS_THAN_EQ,
	TOKEN_GREATER_THAN,
	TOKEN_GREATER_THAN_EQ,
	TOKEN_COMMENT,
	TOKEN_COMMENT_MULTI_START,
	TOKEN_COMMENT_MULTI_END,

	// Boolean operators
	TOKEN_BOOL_AND,
	TOKEN_BOOL_OR,
	TOKEN_BOOL_NOT,
	TOKEN_BOOL_XOR,

	MAX_TOKENS
} token_type;

const char* token_strings[] = {
	"NONE",
	"IDENTIFIER",
	"EQUAL SIGN",
	"NUMBER",
	"PAREN OPEN",
	"PAREN CLOSE",
	"PLUS SIGN",
	"MINUS SIGN",
	"ASTERISK",
	"FORWARD SLASH",
	"SEMI",
	"VALUE",
	"EXPRESSION",
	"KEYWORD",
	// operators
	"DIV EQ",
	"MULT EQ",
	"ADD EQ",
	"SUB EQ",
	"NOT EQ",
	"EQ",
	"LESS THAN",
	"LESS THAN EQ",
	"GREATER THAN",
	"GREATER THAN EQ",
	"COMMENT",
	"COMMENT MULTI START",
	"COMMENT MULTI END",

	// Boolean operators
	"BOOL AND",
	"BOOL OR",
	"BOOL NOT",
	"BOOL XOR",
};

const char* key_words[] = {
	"int", "bool", "long", "double",
	"char", "float", "void", "if", "else",
	"for", "while", "enum", "typedef",
	"struct", "static", "inline", "return",
	"const", "switch", "typedef", "break", "auto",
	"case", "const", "do", "extern", "goto", "short",
	"signed", "sizeof", "unsigned", "union", "volatile",
	"register",
};

typedef enum value_type {
	VAL_NONE = 0,
	VAL_INT,
	VAL_CHAR_STAR
} value_type;

typedef union value_u {
	int integer;
	char* string;
} value_u;

typedef struct value_s {
	value_type type;
	value_u value;
} value_s;

typedef struct token_s {
	token_type type;
	int has_value;
	value_s value;
} token_s;


// Dynamic array of tokens;
typedef struct ast_s {
	long max_tokens;
	long current_token;
	token_s* token_list;
} token_array_s;

token_array_s* init_token_array(void) {
	token_array_s* tkn = malloc(sizeof(token_array_s));
	tkn->max_tokens = 1024;
	tkn->current_token = 0;
	tkn->token_list = malloc(sizeof(token_s) * tkn->max_tokens);
	// Empty out the array
	tkn->token_list = memset(tkn->token_list, TOKEN_NONE, sizeof(token_s) * 1024);

	return tkn;
}

void push_token_val(
		token_array_s* tkn,
		token_type token,
		value_s val
		) {

	// Check for buffer overflows
	if (tkn->current_token + 1 > tkn->max_tokens) {
		tkn->max_tokens += 1024;
		tkn->token_list = realloc(tkn->token_list, tkn->max_tokens);
	}
	
	tkn->token_list[tkn->current_token].type = token;
	if (val.type) {
		tkn->token_list[tkn->current_token].value = val;
		tkn->token_list[tkn->current_token].has_value = 1;
	}
	tkn->current_token++;
}

void push_token(token_array_s* tkn, token_type token) {
	push_token_val(tkn, token, (value_s){0});
}

char* substring(char* string, int start, int len) {
	// replace this with some sort of arena allocation, if necessary
	char* ret = malloc(sizeof(char) * len + 1);
	ret = memcpy(ret, &string[start], len + 1);
	ret[len+1] = '\0';
	return ret;
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

void print_tokens(token_array_s tkn)
{
	// code = strip_whitespace(code);
	printf("===DUMPING TOKENS===\n");
	int i = 0;
	for (i = 0; i < tkn.current_token; i++ ) {
		if (tkn.token_list[i].has_value)
			switch(tkn.token_list[i].value.type) {
				case VAL_INT:
					printf("[%02i] token: %s, value: %i\n", 
							i, 
							token_strings[tkn.token_list[i].type], 
							tkn.token_list[i].value.value.integer);
				break;
				case VAL_CHAR_STAR:
					printf("[%02i] token: %s, value: %s\n", 
							i, 
							token_strings[tkn.token_list[i].type], 
							tkn.token_list[i].value.value.string);
				break;
				default:

				break;
			}
		else  {
			printf("[%02i] TOKEN: %s\n", i, token_strings[tkn.token_list[i].type]);
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current_token - 1, tkn.max_tokens - tkn.current_token);
}


// convert input string into list of tokens
token_array_s* scan(const char* code) {
	token_array_s* tkn = init_token_array();

	// pointers to the start of the string
	// and the current character
	char *c = (char*) code,
			 *start = (char*) code;
	long len = strlen(code);
	int line = 0;

	printf("== Scanning for tokens ==\n");
	for (c = (char*) code; c - start < len; c++) {
		if (char_is_digit(*c)) {
			// skip over numbers
			int num = (*c - '0');
			while(char_is_digit(peek(c))) 
			{
				c++;
				num = (num * 10) + (*c - '0');
			}
			push_token_val(tkn,
					TOKEN_NUMBER, 
					(value_s){ VAL_INT , (value_u) { .integer = num }});
		} else if (char_is_alpha(*c)) {
			// skip over strings
			int str_start = c-start;
			while(char_is_alpha(peek(c))) 
			{
				c++;
				putchar(*c);
			}
			int str_len = c-start-str_start;
			char* substr = substring(start, str_start, str_len);
			push_token_val(
					tkn, 
					TOKEN_IDENTIFIER,
					(value_s){ VAL_CHAR_STAR, (value_u) {.string = substr}});
		}
		else if (char_is_whitespace(*c)){
			if (*c == '\n') line++;
		}
		else {
			switch (*c) {
				case '*':
					if (peek(c) == '=') push_token(tkn, TOKEN_MULT_EQ);
					push_token(tkn, TOKEN_ASTERISK);
				break;
				case '+':
					if (peek(c) == '=') push_token(tkn, TOKEN_ADD_EQ);
					push_token(tkn, TOKEN_PLUS);
				break;
				case '-':
					if (peek(c) == '=') push_token(tkn, TOKEN_SUB_EQ);
					push_token(tkn, TOKEN_MINUS);
				break;
				case '/':
					if (peek(c) == '=') push_token(tkn, TOKEN_DIV_EQ);
					push_token(tkn, TOKEN_FORWARD_SLASH);
				break;
				case '=':
					if (peek(c) == '=') push_token(tkn, TOKEN_EQ);
					push_token(tkn, TOKEN_EQUAL_SIGN);
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
				default:
					putchar(*c);
				break;
			}
		}
	}
	printf("== Scanning complete ==\n");

	return tkn;
}

int
main(void) {
	const char* code = "int thisisanidentifier 1234 * ( 205 + 4 );";
	token_array_s* tkn = scan(code);
	print_tokens(*tkn);
	free(tkn);
}

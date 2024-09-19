#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int char_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static inline int char_is_alpha(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || ( c == '_' );
}

static inline int char_is_alphanum( char c )
{
	return (char_is_alpha(c) || char_is_digit(c));
}

static inline int char_is_whitespace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

/**@brief returns the next character in a string without consuming the current one */
static inline char peek(char* s)
{
	return (*++s);
}

#define NUM_KEY_WORDS 14
const char* key_words[NUM_KEY_WORDS] = {
	"int", 
	"bool",
	"string",
	"return",
	"if",
	"else",
	"while",
	"for",
	"struct",
	"defer",
	"namespace", 
	"print",
	"true",
	"false"
};

typedef enum token_type {
	TKN_NONE = 0,
	// Single Character tokens
	TKN_EQUAL_SIGN,
	TKN_PAREN_OPEN,
	TKN_PAREN_CLOSE,
	TKN_BRACE_OPEN,
	TKN_BRACE_CLOSE,
	TKN_OPEN_QUOTE,
	TKN_CLOSE_QUOTE,
	TKN_COMMA,
	TKN_DOT,
	TKN_PLUS,
	TKN_MINUS,
	TKN_ASTERISK,
	TKN_FORWARD_SLASH,
	TKN_SEMI,

	// keywords
	TKN_INT,
	TKN_BOOL,
	TKN_STRING,
	TKN_RETURN, 
	TKN_IF,
	TKN_ELSE,
	TKN_WHILE,
	TKN_FOR,
	TKN_STRUCT,
	TKN_DEFER,
	TKN_NAMESPACE,
	TKN_PRINT,
	TKN_TRUE,
	TKN_FALSE,

	TKN_VALUE,
	TKN_IDENTIFIER,
	TKN_INTEGER_LITERAL,
	TKN_FLOATING_LITERAL,
	TKN_CHAR_LITERAL,
	TKN_STRING_LITERAL,

	// operators
	TKN_BANG,
	TKN_BANG_EQ,
	TKN_DIV_EQ,
	TKN_MULT_EQ,
	TKN_ADD_EQ,
	TKN_SUB_EQ,
	TKN_NOT_EQ,
	TKN_EQ_EQ,
	TKN_LESS_THAN,
	TKN_LESS_THAN_EQ,
	TKN_GREATER_THAN,
	TKN_GREATER_THAN_EQ,

	TKN_EOF,

	MAX_TKNS
} token_type;

const char* token_strings[] = {
	"NONE",
	// Single Character tokens
	"TKN_EQUAL_SIGN",
	"TKN_PAREN_OPEN",
	"TKN_PAREN_CLOSE",
	"TKN_BRACE_OPEN",
	"TKN_BRACE_CLOSE",
	"OPEN QUOTE",
	"CLOSE QUOTE",
	"TKN_COMMA",
	"TKN_DOT",
	"TKN_PLUS",
	"TKN_MINUS",
	"TKN_ASTERISK",
	"TKN_FORWARD_SLASH",
	"TKN_SEMI",

	// keywords
	"INT",
	"BOOL",
	"STRING",
	"RETURN", 
	"IF",
	"ELSE",
	"WHILE",
	"FOR",
	"STRUCT",
	"DEFER",
	"NAMESPACE",
	"PRINT",
	"TRUE",
	"FALSE",

	"VALUE",
	"IDENTIFIER",
	"INTEGER LITERAL",
	"FLOATING LITERAL",
	"CHAR LITERAL",
	"STRING LITERAL",

	// operators
	"TKN_BANG",
	"TKN_BANG_EQ",
	"TKN_DIV_EQ",
	"TKN_MULT_EQ",
	"TKN_ADD_EQ",
	"TKN_SUB_EQ",
	"TKN_NOT_EQ",
	"TKN_EQ_EQ",
	"TKN_LESS_THAN",
	"TKN_LESS_THAN_EQ",
	"TKN_GREATER_THAN",
	"TKN_GREATER_THAN_EQ",

	"TKN_EOF",
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
	int line, offset, len;
} value_s;

typedef struct token_s {
	token_type type;
	int has_value;
	int chr_index; // the starting character index
	value_s value;
} token_s;

typedef struct str_pos {
	int start, len, 
			line;
} str_pos;


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
	tkn->token_list = memset(tkn->token_list, TKN_NONE, sizeof(token_s) * 1024);

	return tkn;
}

void push_token_val(
		token_array_s* tkn,
		token_type token,
		value_s val,
		int chr_index
		) {

	// Check for buffer overflows
	if (tkn->current_token + 1 > tkn->max_tokens) {
		tkn->max_tokens += 1024;
		tkn->token_list = realloc(tkn->token_list, tkn->max_tokens);
	}
	
	tkn->token_list[tkn->current_token].type = token;
	tkn->token_list[tkn->current_token].chr_index = chr_index;
	if (val.type) {
		tkn->token_list[tkn->current_token].value = val;
		tkn->token_list[tkn->current_token].has_value = 1;
	}
	tkn->current_token++;
}

void push_token(token_array_s* tkn, token_type token, int chr_index) {
	push_token_val(tkn, token, (value_s){0}, chr_index);
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

void print_token_array(token_array_s tkn)
{
	// code = strip_whitespace(code);
	printf("===DUMPING TKNS===\n");
	printf("[TI:CI] token[TN]: tkn, value: val\n"
				 "----------------------------------\n");
	int i = 0;
	for (i = 0; i < tkn.current_token; i++ ) {
		token_s current = tkn.token_list[i];
		if (current.has_value)
			switch(current.value.type) {
				case VAL_INT:
					printf("[%02i:%02i] token[%02i]: %s, value: %i\n", 
							i, 
							current.chr_index,
							current.type,
							token_strings[current.type], 
							current.value.value.integer);
				break;
				case VAL_CHAR_STAR:
					printf("[%02i:%02i] token[%02i]: %s, value: %s\n", 
							i, 
							current.chr_index,
							current.type,
							token_strings[current.type], 
							current.value.value.string);
				break;
				default:

				break;
			}
		else  {
			printf("[%02i:%02i] token[%02i]: %s\n",
					i,
					current.chr_index,
					current.type,
					token_strings[current.type]);
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current_token - 1, tkn.max_tokens - tkn.current_token);
}

// convert input string into list of tokens
token_array_s* tokenize (const char* code) {
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
			if (peek(c) == '.') {
				
			}
			push_token_val(tkn,
					TKN_INTEGER_LITERAL, 
					(value_s){ VAL_INT , (value_u) { .integer = num }},
					c - start
					);
		} else if (char_is_alpha(*c)) {
			// skip over strings
			int str_start = c-start;
			while(char_is_alpha(peek(c))) 
			{
				c++;
			}
			int str_len = c-start-str_start;
			char* substr = substring(start, str_start, str_len);

			// This flag is here because i couldn't figure out the
			// proper control flow needed to make sure that keyword
			// identifiers get stored as keyword tokens and not keyword AND
			// identifier tokens. This flag is my little hack.
			int flag = 0;
			for(int i = 0; i < NUM_KEY_WORDS; i++) {
				if (!strcmp(substr, key_words[i])) {
					
					token_type token = i + TKN_INT;

					push_token_val(
					tkn, 
					token,
					(value_s){ VAL_CHAR_STAR, (value_u) {.string = substr}}, 
					c - start
					);
					flag = 1;
				}
			}
			if (!flag)
			push_token_val(
					tkn, 
					TKN_IDENTIFIER,
					(value_s){ VAL_CHAR_STAR, (value_u) {.string = substr}},
					c - start
					);
		}
		else if (char_is_whitespace(*c)){
			if (*c == '\n') line++;
		}
		else {
			switch (*c) {
				case '(':
					push_token(tkn, TKN_PAREN_OPEN, c - start);
				break;
				case ')':
					push_token(tkn, TKN_PAREN_CLOSE, c - start);
				break;
				case '{':
					push_token(tkn, TKN_BRACE_OPEN, c - start);
				break;
				case '}':
					push_token(tkn, TKN_BRACE_CLOSE, c - start);
				break;
				case ';':
					push_token(tkn, TKN_SEMI, c - start);
				break;
				case ',':
					push_token(tkn, TKN_COMMA, c - start);
				break;
				case '.':
					push_token(tkn, TKN_DOT, c - start);
				break;
				case '*':
					if (peek(c) == '=') push_token(tkn, TKN_MULT_EQ, c - start);
					else push_token(tkn, TKN_ASTERISK, c - start);
				break;
				case '+':
					if (peek(c) == '=') push_token(tkn, TKN_ADD_EQ, c - start);
					else push_token(tkn, TKN_PLUS, c - start);
				break;
				case '-':
					if (peek(c) == '=') push_token(tkn, TKN_SUB_EQ, c - start);
					else push_token(tkn, TKN_MINUS, c - start);
				break;
				case '/':
					if (peek(c) == '=') { 
						push_token(tkn, TKN_DIV_EQ, c - start);
						c++;
					} 
					// COMMENT
					else if  (peek(c) == '/')
					{
						while(peek(c) != '\n') c++;
					}
					else push_token(tkn, TKN_FORWARD_SLASH, c - start);
				break;
				case '!':
					if (peek(c) == '=') {
						push_token(tkn, TKN_BANG_EQ, c - start);
						c++;
					}
					else push_token(tkn, TKN_BANG, c - start);
				break;
				case '=':
					if (peek(c) == '=')
					{
						push_token(tkn, TKN_EQ_EQ, c - start);
						c++;
					}
					else push_token(tkn, TKN_EQUAL_SIGN, c - start);
				break;
				case '<':
					if (peek(c) == '=') {
						push_token(tkn, TKN_LESS_THAN_EQ, c - start);
						c++;
					}
					else push_token(tkn, TKN_LESS_THAN, c - start);
				break;
				case '>':
					if (peek(c) == '=')
					{
						push_token(tkn, TKN_GREATER_THAN_EQ, c - start);
						c++;
					}
					else push_token(tkn, TKN_GREATER_THAN, c - start);
				break;
				default:
					// putchar(*c);
					printf("Unknown character encountered: %c at l:%i\n", *c,
							line);
				break;
			}
		}
	}
	push_token(tkn, TKN_EOF, len);
	printf("== Scanning complete ==\n");

	return tkn;
}

int
main(void) {
	const char* code = "x = 34 * ( 224 / 32 + 24 );";
	token_array_s* tkn = tokenize(code);
	print_token_array(*tkn);
	free(tkn);
}

#include "scanner.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct scanner_s {
	char *start;
	int line;
};

static struct scanner_s scanner;

#define MAKE_LIT_STR(x) (literal_s){ LIT_STRING , (literal_u) { ._str = x }}
#define MAKE_LIT_INT(x) (literal_s){ LIT_INT , (literal_u) { ._int = x }}
#define MAKE_LIT_FLOAT(x) (literal_s){ LIT_FLOAT , (literal_u) { ._float = x }}
#define MAKE_LIT_DOUBLE(x) (literal_s){ LIT_DOUBLE , (literal_u) { ._double = x }}

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

// returns the next character in a string without consuming the current one
static inline char peek(char* s)
{
	return (*++s);
}

static inline int line() {
	return scanner.line;
}

static tokcat tok_to_cat(token_type token) {
	if ( in_range_inc(token, T_EQUAL_SIGN, T_HAT) ) 
	{
		return TC_SYMBOL;
	}
	if ( in_range_inc(token, T_DIV_EQ, T_SHR) ) {
		return TC_OPERATOR;
	}
	if ( in_range_inc(token, T_INTEGER_LITERAL, T_NULL) )  {
		return TC_LITERAL;
	}
	if ( in_range_inc(token, T_UBYTE, T_FN) ) {
		return TC_KEYWORD;
	}	
	if ( token == T_IDENTIFIER ) return TC_ID; 
	return TC_NONE;
}

static token_array_s* init_token_array(void)
{
	token_array_s* tkn = malloc(sizeof(token_array_s));
	tkn->max_tokens = 512;
	tkn->current_token = 0;
	tkn->token_list = malloc(sizeof(token_s) * tkn->max_tokens);
	// Empty out the array
	tkn->token_list = memset(tkn->token_list, 0, sizeof(token_s) * tkn->max_tokens);

	tkn->max_literals = 512;
	tkn->current_literal = 0;
	tkn->literal_list = malloc(sizeof(literal_s) * tkn->max_literals);
	tkn->literal_list = memset(tkn->literal_list, 0, sizeof(literal_s) * tkn->max_literals);

	return tkn;
}

static void push_token_lit (
		token_array_s* tkn,
		token_type token,
		substr_s source,
		literal_s lit,
		int chr_index
	) 
{
	// Check for buffer overflows
	if (tkn->current_token + 1 > tkn->max_tokens) {
		tkn->max_tokens += 1024;
		tkn->token_list = realloc(tkn->token_list, tkn->max_tokens);
	}
	if (tkn->current_literal + 1 > tkn->max_literals) {
		tkn->max_literals += 1024;
		tkn->literal_list = realloc(tkn->literal_list, tkn->max_literals);
	}
	
	tkn->token_list[tkn->current_token].type = token;
	tkn->token_list[tkn->current_token].cat = tok_to_cat(token);
	tkn->token_list[tkn->current_token].chr_index = chr_index;
	tkn->token_list[tkn->current_token].source = source;
	tkn->token_list[tkn->current_token].line = line();
	if (lit.type != LIT_NONE) {
		tkn->literal_list[tkn->current_literal] = lit;
		tkn->token_list[tkn->current_token].literal_id = tkn->current_literal;
		tkn->token_list[tkn->current_token].has_literal = 1;

		tkn->current_literal++;
	}

	if (!(token == T_EOF))
		tkn->current_token++;
}

static void push_token(token_array_s* tkn, token_type token, substr_s source, int chr_index ) 
{
	push_token_lit(tkn, token, source, (literal_s){.type = LIT_NONE}, chr_index);
}

// convert input string into list of tokens
token_array_s* tokenize ( string_s src )
{
	// TODO: refactor this mess into something that's less of a mess
	token_array_s* tkn = init_token_array();

	// pointers to the start of the string
	// and the current character
	char *start = (char*) src.string;
	size_t len = src.size;
	scanner.line = 1;
	char* line_start = start;

	// printf("Scanning %s for tokens ...\n", src.path);
	for (char *c = (char*) src.string; c - start < len; c++) {
		if (char_is_digit(*c)) {

			int _start = c-start;
			char* str_ptr = c;
			// skip over numbers
			int num = (*c - '0');
			// literal_type lit_type = LIT_INT;
			while(char_is_digit(peek(c))) 
			{
				c++;
				num = (num * 10) + (*c - '0');
			}
			if (peek(c) == '.') {
				c++;
				c++;
				if (!char_is_digit(*c) && !(*c == 'f') && !(*c == 'd')) {
					printf("ERROR: extra period after number %i at line %i\n", num, line());
				}
				else {
					double pnum = 0;
					int level = 1;
					while(char_is_digit(*c)) 
					{
						double temp = 0;
						// printf("char: %c\n", *c);
						temp = (*c - '0');
						for (int i = 0; i < level; i++) temp /= 10;
						c++;
						level++;
						pnum += temp;
					}
					pnum += num;
					int len = c-start-_start;

					// printf("PNUM: %f\n", pnum);
					push_token_lit(tkn,
							T_DOUBLE_FLOATING_LITERAL, 
							(substr_s){.len = len, .c_ptr = str_ptr},
							MAKE_LIT_DOUBLE(pnum),
							c - line_start
							);
				}
				continue;
			}
			int len = c-start-_start;
			push_token_lit(tkn,
					T_INTEGER_LITERAL, 
					(substr_s){.len = len + 1, .c_ptr = str_ptr},
					(literal_s){ LIT_INT , (literal_u) { ._int = num }},
					c - line_start
					);
		} else if (char_is_alpha(*c)) {
			int str_start = c-start;
			char* str_ptr = c;
			while(char_is_alpha(peek(c))) 
			{
				c++;
			}
			int str_len = c-start-str_start + 1;
			substr_s substr = (substr_s){.c_ptr = str_ptr, .len = str_len};

			// This flag is here because i couldn't figure out the
			// proper control flow needed to make sure that keyword
			// identifiers get stored as keyword tokens and not keyword AND
			// identifier tokens. This flag is my little hack.
			int flag = 0;
			for(int i = 0; i < NUM_KEY_WORDS; i++) {
				int id = i + T_TRUE;
				if (
						substr.len == strlen(token_strings[id]) &&
						!memcmp(substr.c_ptr, token_strings[id], substr.len)) {
					token_type token = id;

					push_token(
					tkn, 
					token,
					substr,
					c - line_start
					);
					flag = 1;
				}
			}
			if (!flag)
			push_token_lit(
					tkn, 
					T_IDENTIFIER,
					substr,
					MAKE_LIT_STR(substr),
					c - line_start
					);
		}
		else if (char_is_whitespace(*c)){
			if (*c == '\n') {
				scanner.line++;
				line_start = c;
			}
		}
		else {
			switch (*c) {
				case '(':
					push_token(tkn, T_PAREN_OPEN, (substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ')':
					push_token(tkn, T_PAREN_CLOSE, (substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '{':
					push_token(tkn, T_BRACE_OPEN, (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '}':
					push_token(tkn, T_BRACE_CLOSE,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '[':
					push_token(tkn, T_BRACKET_OPEN, (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case ']':
					push_token(tkn, T_BRACKET_CLOSE,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ';':
					push_token(tkn, T_SEMI,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ',':
					push_token(tkn, T_COMMA,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '?':
					push_token(tkn, T_QUESTION,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ':':
					push_token(tkn, T_COLON,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '~':
					push_token(tkn, T_TILDE,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '^':
					push_token(tkn, T_HAT,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '&':
					if (peek(c) == '=')
					{
						push_token(tkn, T_AND_EQ, (substr_s){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					} else if (peek(c) == '&') {
						push_token(tkn, T_LOG_AND, (substr_s){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else push_token(tkn, T_AMP,(substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '|':
					if (peek(c) == '=')
					{
						push_token(tkn, T_OR_EQ, (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (peek(c) == '|') {
						push_token(tkn, T_LOG_OR, (substr_s){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else push_token(tkn, T_PIPE, (substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '.':
					if (peek(c) == '.') {
						push_token(tkn, T_DOT_DOT,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_DOT,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '*':
					if (peek(c) == '=') {
						push_token(tkn, T_MULT_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_ASTERISK,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '+':
					if (peek(c) == '=') {
						push_token(tkn, T_ADD_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (peek(c) == '+') {
						push_token(tkn, T_INC,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_PLUS,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '-':
					if (peek(c) == '=') {
						push_token(tkn, T_SUB_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} 
					else if (peek(c) == '-') {
						push_token(tkn, T_DEC,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (peek(c) == '>') {
						push_token(tkn, T_ARROW,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_MINUS,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '/':
					if (peek(c) == '=') { 
						push_token(tkn, T_DIV_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} 
					// COMMENT
					else if  (peek(c) == '/')
					{
						while(peek(c) != '\n') {
							c++;
						}
						line_start = c;
					}
					else push_token(tkn, T_FORWARD_SLASH,  (substr_s){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '%':
					if (peek(c) == '=') {
						push_token(tkn, T_MOD_EQ, (substr_s){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else push_token(tkn, T_MODULUS,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '!':
					if (peek(c) == '=') {
						push_token(tkn, T_NOT_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_BANG,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '=':
					if (peek(c) == '=')
					{
						push_token(tkn, T_EQ_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (peek(c) == '>') {
						push_token(tkn, T_FAT_ARROW,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else push_token(tkn, T_EQUAL_SIGN,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '<':
					if (peek(c) == '=') {
						push_token(tkn, T_LESS_THAN_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (peek(c) == '<' ) {
						push_token(tkn, T_SHL,  (substr_s){.len = 2, .c_ptr = c},c-line_start);
						c++;
					}
					else push_token(tkn, T_LESS_THAN,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '>':
					if (peek(c) == '=')
					{
						push_token(tkn, T_GREATER_THAN_EQ,  (substr_s){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (peek(c) == '>' ) {
						push_token(tkn, T_SHR,  (substr_s){.len = 2, .c_ptr = c},c-line_start);
						c++;
					}
					else push_token(tkn, T_GREATER_THAN,  (substr_s){.len = 1, .c_ptr = c},c - line_start);
				break;
				// STRING HANDLING
				case '"':
				{
					if(peek(c) == '"') {

					push_token_lit(
							tkn,
							T_STRING_LITERAL,
						 (substr_s){.len = 2, .c_ptr = c},
							(literal_s) {
								.type = LIT_STRING,
								.literal = (literal_u) {
									._str = (substr_s){.c_ptr = c, .len = 1}
								}
							},
							c - line_start);
					c += 2;
					}
					int str_start = c-start;
					char* str_ptr = c;
					while( peek(c) != '"' && ((c - start) != len) ) {
						if ( peek(c) == '\n' ) {
							c++;
							printf("WARNING: Unterminated string on a newline at line %i\n", line());
							scanner.line++;
							// continue;
						}
						c++;
					}

					if ( (c-start) == len ) {
						printf("ERROR: Unterminated string\n");
						// exit(-1);
					}
					c++;

					int len = c-start-str_start;
					// char* substr = substring(start, str_start, c-start-str_start);
					substr_s substr = (substr_s){ .c_ptr = str_ptr + 1, .len = len - 1 };
					push_token_lit(
							tkn,
							T_STRING_LITERAL, 
							(substr_s){.len = len + 1, .c_ptr = str_ptr},
							(literal_s) {
								.type = LIT_STRING,
								.literal = (literal_u) {
									._str = substr
								}
							},
							c - line_start);
					break;
				}
				case '\'':
				{
					char* str_ptr = c;
					if(peek(++c) != '\'') {
						printf("ERROR, unterminated character\n");
						c++;
						continue;
					}
					else {
						push_token_lit(
								tkn,
								T_CHAR_LITERAL,
								(substr_s){.len = 3, .c_ptr = str_ptr},
								(literal_s){
								.type = LIT_CHAR,
								.literal = (literal_u) {
									._char = *c
								}
								},
								c - line_start 
								);
						c++;
					}
				}
				break;
				default:
					printf("Unknown character encountered: %c at l:%i\n", *c,
							line());
				break;
			}
		}
	}
	push_token(tkn, T_EOF, (substr_s){.len = 0, .c_ptr = scanner.start + len}, len);

	return tkn;
}

void print_token_array(string_s src, token_array_s tkn)
{
	printf("===DUMPING TS===\n");
	// printf("File: %s\n", src.path);
	// printf("[TI:CI] token[TN]: tkn, value: val\n"
				 // "----------------------------------\n");
	int i = 0;
	for (i = 0; i <= tkn.current_token; i++ ) {
		token_s current = tkn.token_list[i];
		if (current.has_literal) {
			literal_s cur_lit = tkn.literal_list[current.literal_id];
			literal_u lit = cur_lit.literal;
			int chid = current.chr_index;
			token_type type = current.type;
			switch(cur_lit.type) {
				case LIT_INT:
					printf("[%02i:%02i]\n source: %.*s\n\t token[%02i]: %s, value: %02i\n", 
							i, 
							chid,
							current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							// current.literal_id,
							lit._int
							);
				break;
				case LIT_DOUBLE:
					printf("[%02i:%02i]\n source: %.*s\n\t token[%02i]: %s, value: %lf\n", 
							i, 
							chid,
							current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							// current.literal_id,
							lit._double
							);
				break;
				case LIT_FLOAT:
					printf("[%02i:%02i]\t source: %.*s\n\t token[%02i]: %s, value: %f\n", 
							i, 
							chid,
							current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							// current.literal_id,
							lit._float
							);
				break;
				case LIT_STRING:
					printf("[%02i:%02i]\t source: %.*s\n\t token[%02i]: %s, value: %.*s\n", 
							i, 
							chid,
							current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							cur_lit.literal._str.len,
							cur_lit.literal._str.c_ptr
							);
				break;
				case LIT_CHAR:
					printf("[%02i:%02i]\t source: %.*s\n\t token[%02i]: %s, value: %c\n", 
							i, 
							chid,
							current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							cur_lit.literal._char);
				default:

				break;
			}
		}
		else  {
			printf("[%02i:%02i]\t source: %.*s\n\t token[%02i]: %s\n", 
					i,
					current.chr_index,
					current.source.len,
					current.source.c_ptr,
					current.type,
					token_to_str(current.type));
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current_token, tkn.max_tokens - tkn.current_token);
	printf("Total used literals: %ld, Unused literals: %ld\n", tkn.current_literal, tkn.max_literals - tkn.current_literal);
}



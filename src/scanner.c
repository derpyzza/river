#include "scanner.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Scanner {
	int line;
	char *start;
};

static struct Scanner scanner;

#define MAKE_LIT_STR(x) (Literal){ LIT_STRING , (LitUnion) { .string = x }}
#define MAKE_LIT_INT(x) (Literal){ LIT_INT , (LitUnion) { .integer = x }}
#define MAKE_LIT_FLOAT(x) (Literal){ LIT_FLOAT , (LitUnion) { .floating = x }}

static inline bool _is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static inline bool _is_alpha(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || ( c == '_' );
}

static inline bool _is_alphanum( char c )
{
	return (_is_alpha(c) || _is_digit(c));
}

static inline bool _is_whitespace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

// returns the next character in a string without consuming the current one
static inline char _peek(char* s)
{
	return (*++s);
}

static inline int _line(void) {
	return scanner.line;
}

CatType tok_to_cat(TokenType token) {
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
	if ( in_range_inc(token, T_UBYTE, T_LET) ) {
		return TC_KEYWORD;
	}	
	if ( token == T_IDENTIFIER ) return TC_ID; 
	return TC_NONE;
}

static TokenArray* init_token_array(void) {
	TokenArray* tkn = malloc(sizeof(*tkn));
	tkn->max_tokens = 512;
	tkn->current_token = 0;
	tkn->token_list = malloc(sizeof(*tkn->token_list) * tkn->max_tokens);
	// Empty out the array
	tkn->token_list = memset(tkn->token_list, 0, sizeof(*tkn->token_list) * tkn->max_tokens);

	tkn->max_literals = 512;
	tkn->current_literal = 0;
	tkn->literal_list = malloc(sizeof(*tkn->literal_list) * tkn->max_literals);
	tkn->literal_list = memset(tkn->literal_list, 0, sizeof(*tkn->literal_list) * tkn->max_literals);

	return tkn;
}

static void _push_token_lit (
		TokenArray* tkn,
		TokenType token,
		String source,
		Literal lit,
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
	tkn->token_list[tkn->current_token].line = _line();
	if (lit.type != LIT_NONE) {
		tkn->literal_list[tkn->current_literal] = lit;
		tkn->token_list[tkn->current_token].literal_id = tkn->current_literal;
		tkn->token_list[tkn->current_token].has_literal = True;

		tkn->current_literal++;
	}

	if (!(token == T_EOF))
		tkn->current_token++;
}

static void _push_token( 
  	TokenArray* tkn,
    TokenType token,
    String source,
    int chr_index 
  )
{
	_push_token_lit(tkn, token, source, (Literal){.type = LIT_NONE}, chr_index);
}

// convert input string into list of tokens
struct TokenArray* tokenize( String *src )
{
	// TODO: refactor this mess into something that's less of a mess
	TokenArray* tkn = init_token_array();

	// pointers to the start of the string
	// and the current character
	char *start = (char*) src->c_ptr;
	size len = src->len;
	scanner.line = 1;
	char* line_start = start;

	// printf("Scanning %s for tokens ...\n", src.path);
	for (char *c = (char*) src->c_ptr; c - start < len; c++) {
		if (_is_digit(*c)) {

			int _start = c-start;
			char* str_ptr = c;
			// skip over numbers
			ilong num = (*c - '0');
			// literal_type lit_type = LIT_INT;
			while(_is_digit(_peek(c))) 
			{
				c++;
				num = (num * 10) + (*c - '0');
			}
			if (_peek(c) == '.') {
				c++;
				c++;
				if (!_is_digit(*c) && !(*c == 'f') && !(*c == 'd')) {
					printf("ERROR: extra period after number %li at line %i\n", num, _line());
				}
				else {
					double pnum = 0;
					int level = 1;
					while(_is_digit(*c)) 
					{
						double temp = 0;
						temp = (*c - '0');
						for (int i = 0; i < level; i++) temp /= 10;
						c++;
						level++;
						pnum += temp;
					}
					pnum += num;
					int len = c-start-_start;

					_push_token_lit(tkn,
							T_FLOATING_LITERAL, 
							(String){.len = len, .c_ptr = str_ptr},
							MAKE_LIT_FLOAT(pnum),
							c - line_start
							);
				}
				continue;
			}
			int len = c-start-_start;
			_push_token_lit(tkn,
					T_INTEGER_LITERAL, 
					(String){.len = len + 1, .c_ptr = str_ptr},
					(Literal){ LIT_INT , (LitUnion) { .integer = num }},
					c - line_start
					);
		} else if (_is_alpha(*c)) {
			int str_start = c-start;
			char* str_ptr = c;
			// the first letter of an identifier MUST be an alphabetical character, but every subsequent character can be alphanumeric.
			// so _1234 is a valid identifier but 12t is not for example.
			while(_is_alphanum(_peek(c))) 
			{
				c++;
			}
			int str_len = c-start-str_start + 1;
			String substr = (String){.c_ptr = str_ptr, .len = str_len};

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
					TokenType token = id;

					_push_token(
					tkn, 
					token,
					substr,
					c - line_start
					);
					flag = 1;
				}
			}
			if (!flag)
			_push_token_lit(
					tkn, 
					T_IDENTIFIER,
					substr,
					MAKE_LIT_STR(substr),
					c - line_start);
		}
		else if (_is_whitespace(*c)){
			if (*c == '\n') {
				scanner.line++;
				line_start = c;
			}
		}
		else {
			switch (*c) {
				case '(':
					_push_token(tkn, T_PAREN_OPEN, (String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ')':
					_push_token(tkn, T_PAREN_CLOSE, (String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '{':
				_push_token(tkn, T_BRACE_OPEN, (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '}':
				_push_token(tkn, T_BRACE_CLOSE,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '[':
				_push_token(tkn, T_BRACKET_OPEN, (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case ']':
				_push_token(tkn, T_BRACKET_CLOSE,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ';':
				_push_token(tkn, T_SEMI,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ',':
				_push_token(tkn, T_COMMA,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '?':
				_push_token(tkn, T_QUESTION,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case ':':
				_push_token(tkn, T_COLON,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '~':
				_push_token(tkn, T_TILDE,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '^':
				_push_token(tkn, T_HAT,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '&':
					if (_peek(c) == '=')
					{
					_push_token(tkn, T_AND_EQ, (String){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					} else if (_peek(c) == '&') {
					_push_token(tkn, T_LOG_AND, (String){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else _push_token(tkn, T_AMP,(String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '|':
					if (_peek(c) == '=')
					{
					_push_token(tkn, T_OR_EQ, (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (_peek(c) == '|') {
					_push_token(tkn, T_LOG_OR, (String){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else _push_token(tkn, T_PIPE, (String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '.':
					if (_peek(c) == '.') {
					_push_token(tkn, T_DOT_DOT,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_DOT,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '*':
					if ( _peek(c) == '=') {
					_push_token(tkn, T_MULT_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_ASTERISK,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '+':
					if (_peek(c) == '=') {
					_push_token(tkn, T_ADD_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (_peek(c) == '+') {
					_push_token(tkn, T_INC,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_PLUS,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '-':
					if (_peek(c) == '=') {
					_push_token(tkn, T_SUB_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} 
					else if (_peek(c) == '-') {
					_push_token(tkn, T_DEC,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (_peek(c) == '>') {
					_push_token(tkn, T_ARROW,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_MINUS,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '/':
					if (_peek(c) == '=') { 
					_push_token(tkn, T_DIV_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} 
					// COMMENT
					else if  (_peek(c) == '/')
					{
						while(_peek(c) != '\n') {
							c++;
						}
						line_start = c;
					}
					else _push_token(tkn, T_FORWARD_SLASH,  (String){.len = 1, .c_ptr = c}, c - line_start);
				break;
				case '%':
					if (_peek(c) == '=') {
					_push_token(tkn, T_MOD_EQ, (String){.len = 2, .c_ptr = c}, c - line_start);
						c++;
					}
					else _push_token(tkn, T_MODULUS,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '!':
					if (_peek(c) == '=') {
					_push_token(tkn, T_NOT_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_BANG,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '=':
					if (_peek(c) == '=')
					{
					_push_token(tkn, T_EQ_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					} else if (_peek(c) == '>') {
					_push_token(tkn, T_FAT_ARROW,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else _push_token(tkn, T_EQUAL_SIGN,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '<':
					if (_peek(c) == '=') {
					_push_token(tkn, T_LESS_THAN_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (_peek(c) == '<' ) {
					_push_token(tkn, T_SHL,  (String){.len = 2, .c_ptr = c},c-line_start);
						c++;
					}
					else _push_token(tkn, T_LESS_THAN,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				case '>':
					if (_peek(c) == '=')
					{
					_push_token(tkn, T_GREATER_THAN_EQ,  (String){.len = 2, .c_ptr = c},c - line_start);
						c++;
					}
					else if (_peek(c) == '>' ) {
					_push_token(tkn, T_SHR,  (String){.len = 2, .c_ptr = c},c-line_start);
						c++;
					}
					else _push_token(tkn, T_GREATER_THAN,  (String){.len = 1, .c_ptr = c},c - line_start);
				break;
				// STRING HANDLING
				case '"':
				{
					if(_peek(c) == '"') {

				_push_token_lit(
							tkn,
							T_STRING_LITERAL,
						 (String){.len = 2, .c_ptr = c},
							(Literal) {
								.type = LIT_STRING,
								.value = (LitUnion) {
									.string = (String){.c_ptr = c, .len = 1}
								}
							},
							c - line_start);
					c += 2;
					}
					int str_start = c-start;
					char* str_ptr = c;
					while( _peek(c) != '"' && ((c - start) != len) ) {
						if ( _peek(c) == '\n' ) {
							c++;
							printf("WARNING: Unterminated string on a newline at line %i\n", _line());
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
					String substr = (String){ .c_ptr = str_ptr + 1, .len = len - 1 };
				_push_token_lit(
							tkn,
							T_STRING_LITERAL, 
							(String){.len = len + 1, .c_ptr = str_ptr},
							(Literal) {
								.type = LIT_STRING,
								.value = (LitUnion) {
									.string = substr
								}
							},
							c - line_start);
					break;
				}
				case '\'':
				{
					char* str_ptr = c;
					if(_peek(++c) != '\'') {
						printf("ERROR, unterminated character\n");
						c++;
						continue;
					}
					else {
					_push_token_lit(
								tkn,
								T_CHAR_LITERAL,
								(String){.len = 3, .c_ptr = str_ptr},
								(Literal){
								.type = LIT_CHAR,
								.value = (LitUnion) {
									.character = *c
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
							_line());
				break;
			}
		}
	}
_push_token(tkn, T_EOF, (String){.len = 0, .c_ptr = scanner.start + len}, len);

	return tkn;
}

void print_token_array(String *src, TokenArray tkn)
{
	printf("===DUMPING TS===\n");
	// printf("File: %s\n", src.path);
	printf("[TI:CI] token[TN]: tkn, value: val\n"
				 "----------------------------------\n");
	for (int i = 0; i <= tkn.current_token; i++ ) {
		Token current = tkn.token_list[i];
		if (current.has_literal) {
			Literal cur_lit = tkn.literal_list[current.literal_id];
			LitUnion lit = cur_lit.value;
			int chid = current.chr_index;
			TokenType type = current.type;
			switch(cur_lit.type) {
				case LIT_INT:
					printf("[%02i:%02i]\t source: \"%.*s\" token[%02i]: %s, value[id:%i]: %02li\n", 
							i, 
							chid,
							(int)current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							current.literal_id,
							lit.integer
							);
				break;
				case LIT_FLOAT:
					printf("[%02i:%02i]\t source: \"%.*s\" token[%02i]: %s, value[id:%i]: %f\n", 
							i, 
							chid,
							(int)current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							current.literal_id,
							lit.floating
							);
				break;
				case LIT_STRING:
					printf("[%02i:%02i]\t source: \"%.*s\" token[%02i]: %s, value[id:%i]: %.*s\n", 
							i, 
							chid,
							(int)current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							current.literal_id,
							(int)cur_lit.value.string.len,
							cur_lit.value.string.c_ptr
							);
				break;
				case LIT_CHAR:
					printf("[%02i:%02i]\t source: \"%.*s\" token[%02i]: %s, value[id:%i]: %c\n", 
							i, 
							chid,
							(int)current.source.len,
							current.source.c_ptr,
							type,
							token_strings[type], 
							current.literal_id,
							cur_lit.value.character);
				default:

				break;
			}
		}
		else  {
			printf("[%02i:%02i]\t source: \"%.*s\" token[%02i]: %s\n", 
					i,
					current.chr_index,
					(int)current.source.len,
					current.source.c_ptr,
					current.type,
					token_to_str(current.type));
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current_token, tkn.max_tokens - tkn.current_token);
	printf("Total used literals: %ld, Unused literals: %ld\n", tkn.current_literal, tkn.max_literals - tkn.current_literal);
}



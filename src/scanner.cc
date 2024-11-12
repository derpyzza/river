#include "scanner.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// internal scanner state
struct Scanner {
	int line;
	String *src;
	char *file_start;
	char *cur;
	char *tok_start; // start of the current lexeme
};

static struct Scanner scanner;

static inline bool _is_digit( char c = *scanner.cur ) {
	return (c >= '0' && c <= '9');
}

static inline bool _is_alpha( char c = *scanner.cur ) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || ( c == '_' );
}

static inline bool _is_alphanum( char c = *scanner.cur ) {
	return (_is_alpha(c) || _is_digit(c));
}

static inline bool _is_whitespace( char c = *scanner.cur ) {
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

// returns the next character in a string without consuming the current one
static inline char _peek(int by = 1) {
	char *c = scanner.cur;
	return *(c+by);
}

static inline char _cur(void) {
	return *scanner.cur;
}

static inline int _line(void) {
	return scanner.line;
}

static inline int _c_to_int(void) {
	return *scanner.cur - '0';
}

static inline void _advance(int by = 1) {
	scanner.cur += by;
}

TokCat tok_to_cat(TokenType token) {
	if ( in_range_inc(token, T_EQUAL, T_HAT) ) {
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
	TokenArray* tkn = (TokenArray*)malloc(sizeof(*tkn));
	tkn->max = 512;
	tkn->current = 0;
	tkn->tokens = (Token*)malloc(sizeof(*tkn->tokens) * tkn->max);
	// Empty out the array
	tkn->tokens = (Token*)memset(tkn->tokens, 0, sizeof(*tkn->tokens) * tkn->max);
	return tkn;
}


static void _push_token(TokenArray* tknarr, Token tkn) {
	if (tknarr->current + 1 > tknarr->max) {
		tknarr->max *= 2;
		tknarr->tokens = (Token*)realloc(tknarr->tokens, tknarr->max);
	}

	tkn.span.c_ptr = scanner.tok_start;
	tkn.span.len = scanner.cur - scanner.tok_start + 1;

	tknarr->tokens[tknarr->current++] = tkn;
}


static void _push_tok (TokenArray *tkn, TokenType type) {
	_push_token(tkn,
	  (Token){
	  	.type = type	
	  });
}

static void _scan_num(TokenArray *tkn) {

	ilong num = _c_to_int();

	while(_is_digit(_peek())) {
		_advance();
		num = (num * 10) + _c_to_int();
	} 
	
	if (_peek() == '.') {
		_advance(2);
		if (!_is_digit(_cur()) && !(_cur() == 'f') && !(_cur() == 'd')) {
			printf("ERROR: extra period after number %li at line %i\n", num, _line());
		} else {
			long double pnum = 0;
			int level = 1;
			while(_is_digit(_cur())) {
				double temp = 0;
				temp = _c_to_int();
				for (int i = 0; i < level; i++) { temp /= 10; }
				_advance();
				level++;
				pnum += temp;
			}
			pnum += num;
			// int len = c-start-_start;

			_push_tok(tkn, T_FLOATING_LITERAL);
			return;
		}
	}

	// int len = c-start-_start;
	_push_tok(tkn, T_INTEGER_LITERAL);
}

static void _scan_word(TokenArray *tkn) {
	char* str_ptr = scanner.cur;
	// the first letter of an identifier MUST be an alphabetical character, but every subsequent character can be alphanumeric.
	// so _1234 is a valid identifier but 12t is not for example.
	while(_is_alphanum(_peek())) {
		_advance();
	}
	int str_len = scanner.cur-scanner.tok_start + 1;
	String substr = (String){ .len = str_len, .c_ptr = str_ptr };

	// This flag is here because i couldn't figure out the
	// proper control flow needed to make sure that keyword
	// identifiers get stored as keyword tokens and not keyword AND
	// identifier tokens. This flag is my little hack.
	int flag = 0;
	for(int i = 0; i < NUM_KEY_WORDS; i++) {
		int id = i + T_TRUE;
		if ( substr.len == strlen(token_strings[id]) 
		     && !memcmp(substr.c_ptr, token_strings[id], substr.len)) {
			TokenType token = (TokenType) id;
			_push_tok ( tkn, token );
			flag = 1;
		}
	}

	if (!flag) _push_tok ( tkn, T_IDENTIFIER );
}

// convert input string into list of tokens
struct TokenArray* tokenize( String *src ) {
	TokenArray* tkn = init_token_array();
	scanner.src = src;

	// pointers to the start of the string
	// and the current character
	scanner.file_start = (char*) src->c_ptr;
	size len = src->len;
	scanner.line = 1;
	char* line_start = scanner.file_start;
	scanner.cur = src->c_ptr;

	// printf("Scanning %s for tokens ...\n", src.path);
	for (; scanner.cur - scanner.file_start < len; _advance()) {
		scanner.tok_start = scanner.cur;
		if (_is_digit()) {
			_scan_num(tkn);
		} else if (_is_alpha(_cur())) {
			_scan_word(tkn);
		}
		else if (_is_whitespace()){
			if (_cur() == '\n') {
				scanner.line++;
				line_start = scanner.cur;
			}
		}
		else {
			switch (_cur()) {

				case '(':
					_push_tok(tkn, T_PAREN_OPEN);
				break;

				case ')':
					_push_tok(tkn, T_PAREN_CLOSE);
				break;

				case '{':
					_push_tok(tkn, T_BRACE_OPEN);
				break;

				case '}':
					_push_tok(tkn, T_BRACE_CLOSE);
				break;

				case '[':
					_push_tok(tkn, T_BRACKET_OPEN);
				break;

				case ']':
					_push_tok(tkn, T_BRACKET_CLOSE);
				break;

				case ';':
					_push_tok(tkn, T_SEMI);
				break;

				case ',':
					_push_tok(tkn, T_COMMA);
				break;

				case '?':
					_push_tok(tkn, T_QUESTION);
				break;

				case ':':
					_push_tok(tkn, T_COLON);
				break;

				case '~':
					_push_tok(tkn, T_TILDE);
				break;

				case '^':
					_push_tok(tkn, T_HAT);
				break;

				case '&':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_AND_EQ);
					} else if (_peek() == '&') {
						_advance();
						_push_tok(tkn, T_LOG_AND);
					}
					else _push_tok(tkn, T_AMP);
				break;

				case '|':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_OR_EQ);
					} else if (_peek() == '|') {
						_advance();
						_push_tok(tkn, T_LOG_OR);
					}
					else _push_tok(tkn, T_PIPE);
				break;

				case '.':
					if (_peek() == '.') {
						_advance();
						_push_tok(tkn, T_DOT_DOT);
					}
					else _push_tok(tkn, T_DOT);
				break;

				case '*':
					if ( _peek() == '=') {
						_advance();
						_push_tok(tkn, T_MULT_EQ);
					}
					else _push_tok(tkn, T_STAR);
				break;

				case '+':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_ADD_EQ);
					} 
					// else if (_peek(c) == '+') {
					// _push_tok(tkn, T_INC,  (String){.len = 2, .c_ptr = c},c - line_start);
					// 	_advance();
					// }
					else _push_tok(tkn, T_PLUS);
				break;
				case '-':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_SUB_EQ);
					} 
					// else if (_peek(c) == '-') {
					// _push_tok(tkn, T_DEC,  (String){.len = 2, .c_ptr = c},c - line_start);
					// 	_advance();
					// }
					else if (_peek() == '>') {
						_advance();
						_push_tok(tkn, T_ARROW);
					}
					else _push_tok(tkn, T_MINUS);
				break;
				case '/':
					if (_peek() == '=') { 
						_advance();
						_push_tok(tkn, T_DIV_EQ);
					} 
					// COMMENT
					else if  (_peek() == '/') {
						while(_peek() != '\n') {
							_advance();
						}
						line_start = scanner.cur;
					}
					else _push_tok(tkn, T_SLASH);
				break;
				case '%':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_MOD_EQ);
					}
					else _push_tok(tkn, T_MODULUS);
				break;
				case '!':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_NOT_EQ);
					}
					else _push_tok(tkn, T_BANG);
				break;
				case '=':
					if (_peek() == '=')
					{
						_advance();
						_push_tok(tkn, T_EQ_EQ);
					} 
					else if (_peek() == '>') {
						_advance();
						_push_tok(tkn, T_FAT_ARROW);
					}
					else _push_tok(tkn, T_EQUAL);
				break;
				case '<':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_LESS_THAN_EQ);
					}
					else if (_peek() == '<' ) {
						_advance();
						_push_tok(tkn, T_SHL);
					}
					else _push_tok(tkn, T_LESS_THAN);
				break;
				case '>':
					if (_peek() == '=') {
						_advance();
						_push_tok(tkn, T_GREATER_THAN_EQ);
					}
					else if (_peek() == '>' ) {
						_advance();
						_push_tok(tkn, T_SHR);
					}
					else _push_tok(tkn, T_GREATER_THAN);
				break;
				// STRING HANDLING
				case '"':
				{
					if(_peek() == '"') {
						_advance(2);
						_push_tok ( tkn, T_STRING_LITERAL );
					}
					while( _peek() != '"' && ((scanner.cur - scanner.file_start) != len) ) {
						if ( _peek() == '\n' ) {
							_advance();
							printf("WARNING: Unterminated string on a newline at line %i\n", _line());
							scanner.line++;
							// continue;
						}
						_advance();
					}

					if ( (scanner.cur-scanner.file_start) == len ) {
						printf("ERROR: Unterminated string\n");
						// exit(-1);
					}
					_advance();

					// stores the entire string, quotes included
					_push_tok (tkn, T_STRING_LITERAL);
					break;
				}
				case '\'': {
						_advance();
					if(_peek() != '\'') {
						printf("ERROR, unterminated character\n");
						_advance();
						continue;
					} else _advance();
					_push_tok ( tkn, T_CHAR_LITERAL );
					break;
				}
				default:
					printf("Unknown character encountered: %c at l:%i\n", _cur(), _line());
				break;
			}
		}
	}

	_push_tok(tkn, T_EOF);
	return tkn;
}

void print_token_array(String *src, TokenArray tkn)
{
	printf("===DUMPING TS===\n");
	// printf("File: %s\n", src.path);
	printf("[TI:CI] token[TN]: tkn, value: val\n"
				 "----------------------------------\n");
	for (int i = 0; i < tkn.current; i++ ) {
		Token current = tkn.tokens[i];
		switch(current.type) {
			// 	printf("[%02i:%02li]\t source: \"%.*s\" token[%02i]: %s, int: %02li\n", 
			// 		i, 
			// 		current.span.c_ptr - src->c_ptr,
			// 		(int)current.span.len,
			// 		current.span.c_ptr,
			// 		current.type,
			// 		token_strings[current.type], 
			// 		current.ival
			// 		);
			// break;
			// 	printf("[%02i:%02li]\t source: \"%.*s\" token[%02i]: %s, float: %Lf\n", 
			// 		i, 
			// 		current.span.c_ptr - src->c_ptr,
			// 		(int)current.span.len,
			// 		current.span.c_ptr,
			// 		current.type,
			// 		token_strings[current.type], 
			// 		current.fval
			// 		);
			// break;
			case T_INTEGER_LITERAL:
			case T_FLOATING_LITERAL:
			case T_CHAR_LITERAL:
			case T_STRING_LITERAL:
				printf("[%02i:%02li] source: \"%.*s\" token[%02i]: %s, value: %.*s\n", 
					i, 
					current.span.c_ptr - src->c_ptr,
					(int)current.span.len,
					current.span.c_ptr,
					current.type,
					token_strings[current.type], 
					(int)current.span.len,
					current.span.c_ptr
					);
			break;
				// printf("[%02i:%02li]\t source: \"%.*s\" token[%02i]: %s, char: %.*s\n", 
				// 	i, 
				// 	current.span.c_ptr - src->c_ptr,
				// 	(int)current.span.len,
				// 	current.span.c_ptr,
				// 	current.type,
				// 	token_strings[current.type], 
				// 	(int)current.span.len,
				// 	current.span.c_ptr
				// );
			default:
				printf("[%02i:%02li] source: \"%.*s\" token[%02i]: %s\n", 
					i,
					current.span.c_ptr - src->c_ptr,
					(int)current.span.len,
					current.span.c_ptr,
					current.type,
					token_to_str(current.type)
				);
			break;
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current, tkn.max - tkn.current);
}



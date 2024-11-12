#include "scanner.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

// internal scanner state
struct Scanner {
	int line;
	String *src;
	// TokenArray *tkn;
	char *file_start;
	char *cur;
	char *tok_start; // start of the current lexeme
};

static struct Scanner scanner;

static inline bool _is_digit( char c ) {
	return (c >= '0' && c <= '9');
}

static inline bool _is_alpha( char c ) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || ( c == '_' );
}

static inline bool _is_alphanum( char c ) {
	return (_is_alpha(c) || _is_digit(c));
}

static inline bool _is_whitespace( char c ) {
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

// returns the next character in a string without consuming the current one
static inline char _peek(void) {
	char *c = scanner.cur;
	return *(c+1);
}

static inline char _peek_by(int by) {
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

static inline void _advance(void) {
	scanner.cur++;
}

static inline void _advance_by(int by) {
	scanner.cur += by;
}

TokCat tok_to_cat(TokenType token) {
	if ( IN_RANGE_INC(token, T_EQUAL, T_HAT) ) {
		return TC_SYMBOL;
	}
	if ( IN_RANGE_INC(token, T_DIV_EQ, T_SHR) ) {
		return TC_OPERATOR;
	}
	if ( IN_RANGE_INC(token, T_INTEGER_LITERAL, T_NULL) )  {
		return TC_LITERAL;
	}
	if ( IN_RANGE_INC(token, T_RETURN, T_LET) ) {
		return TC_KEYWORD;
	}	
	if ( token == T_IDENTIFIER ) return TC_ID;
	return TC_NONE;
}

static void _push_tok (VecToken *tkn, TokenType type) {
	String span;
	span.c_ptr = scanner.tok_start;
	span.len = scanner.cur - scanner.tok_start + 1;
	vec_push_Token(tkn,
	  (Token){
	  	.span = span,
	  	.type = type	
	  });
}

static void _scan_num(VecToken *tkn) {

	while(_is_digit(_peek())) {
		_advance();
	} 
	
	if (_peek() == '.') {
		_advance_by(2);
		if (!_is_digit(_cur()) && !(_cur() == 'f') && !(_cur() == 'd')) {
			printf("ERROR: extra period after number at line %i\n", _line());
		} else {
			while(_is_digit(_cur())) {
				_advance();
			}
			_push_tok(tkn, T_FLOATING_LITERAL);
			return;
		}
	}

	// int len = c-start-_start;
	_push_tok(tkn, T_INTEGER_LITERAL);
}

static void _scan_word(VecToken *tkn) {
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

// void _init_scanner(TokenArray* tkn, String *src) {
	
// }

// convert input string into list of tokens
VecToken* tokenize( String *src ) {
	VecToken *tkn = new_vec_Token(512);
	// _init_scanner(tkn, src);

	scanner.src = src;

	// pointers to the start of the string
	// and the current character
	scanner.file_start = (char*) src->c_ptr;
	size len = src->len;
	scanner.line = 1;
	char* line_start = scanner.file_start;
	scanner.cur = src->c_ptr;

	// printf("Scanning %s for tokens ...\n", src.path);
	for ( ; scanner.cur - scanner.file_start < len; _advance() ) {
		scanner.tok_start = scanner.cur;

		if ( _is_digit( _cur() ) ) {
			_scan_num(tkn);
		} else if ( _is_alpha( _cur() ) ) {
			_scan_word(tkn);
		} else if ( _is_whitespace( _cur() ) ) {
			if ( _cur() == '\n' ) {
				scanner.line++;
				line_start = scanner.cur;
			}
		} else {
			switch ( _cur() ) {

				case '(': _push_tok(tkn, T_PAREN_OPEN); break;

				case ')': _push_tok(tkn, T_PAREN_CLOSE); break;

				case '{': _push_tok(tkn, T_BRACE_OPEN); break;

				case '}': _push_tok(tkn, T_BRACE_CLOSE); break;

				case '[': _push_tok(tkn, T_BRACKET_OPEN); break;

				case ']': _push_tok(tkn, T_BRACKET_CLOSE); break;

				case ';': _push_tok(tkn, T_SEMI); break;

				case ',': _push_tok(tkn, T_COMMA); break;

				case '?': _push_tok(tkn, T_QUESTION); break;

				case ':': _push_tok(tkn, T_COLON); break;

				case '~': _push_tok(tkn, T_TILDE); break;

				case '^': _push_tok(tkn, T_HAT); break;

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
						_advance_by(2);
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

void print_token_array(String *src, VecToken tkn) {
	printf("===DUMPING TS===\n");
	// printf("File: %s\n", src.path);
	printf("[TI:CI] token[TN]: tkn, value: val\n"
				 "----------------------------------\n");
	for (int i = 0; i < tkn.current; i++ ) {
		Token current = tkn.data[i];
		switch(current.type) {
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

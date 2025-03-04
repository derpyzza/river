#include "scanner.h"

#include <stdio.h>
#include <string.h>

// internal scanner state
struct Scanner {
	int line;
	dstr *src;
	dbuf_token *tkns;
	char *file_start;
	char *cur;
	char *tok_start; // start of the current lexeme
};

const char* tok_to_str(int t) {
	switch((TokenTag)t) {
		#define TKN(t, s) case t: return s; break;
		#define MRK(t)
			TKNS
		#undef TKN
		case T_NONE: return "<none>"; break;
		case TKNS_MAX: case TKNS_START:
		case T_KEYWORD_END: case T_KEYWORD_START: return "<undefined token>"; break;
	}
}

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

static inline u8 _cur(void) {
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

static void _push_tok (int type) {
	dstr span;
	span.cptr = scanner.tok_start;
	span.len = scanner.cur - scanner.tok_start + 1;
	dbuf_push_token(scanner.tkns,
	  (Token){
	  	.span = span,
	  	.type = type
	  });
}

/*
	number -> ( <integer> | <float> ) <exponent>? ;

	exponent -> ('e'|'E') <[-+]>? <integer>;

	integer ->
		\d* 
		| \d+ <[_']>? \d+ 
		| <hex_literal>
		| <octal_literal>
		| <binary_literal> ;

	float -> \d* ( '.' \d+ )? <[fFdD]> ;

	hex_literal -> '0x' [1..9]* | ['a'..'f']* | ['A'..'F'] ;
	octal_literal -> '0c' [1..7]* ;
	binary_literal -> '0b' <[01]>* ;
*/

static void _scan_num(dbuf_token *tkn) {

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
			_push_tok(T_LIT_FLOAT);
			return;
		}
	}

	// int len = c-start-_start;
	_push_tok(T_LIT_INT);
}

static void _scan_word(dbuf_token *tkn) {
	char* str_ptr = scanner.cur;
	// the first letter of an identifier MUST be an alphabetical character, but every subsequent character can be alphanumeric.
	// so _1234 is a valid identifier but 12t is not for example.
	while(_is_alphanum(_peek())) {
		_advance();
	}
	int str_len = scanner.cur-scanner.tok_start + 1;
	dstr substr = dstr(str_len, str_ptr);

	// This flag is here because i couldn't figure out the
	// proper control flow needed to make sure that keyword
	// identifiers get stored as keyword tokens and not keyword AND
	// identifier tokens. This flag is my little hack.
	int flag = 0;
	for(int i = 0; i < NUM_KEY_WORDS; i++) {
		int id = i + T_TRUE;
		const char * str = tok_to_str(id);
		if ( substr.len == strlen(str)
		     && !memcmp(substr.cptr, str, substr.len)) {
			TokenTag token = (TokenTag) id;
			_push_tok ( token );
			flag = 1;
		}
	}

	if (!flag) _push_tok ( T_IDEN );
}

// void _init_scanner(TokenArray* tkn, String *src) {
	
// }

// convert input string into list of tokens
dbuf_token* tokenize( dstr *src ) {
	dbuf_token *tkn = dbuf_new_token(512);
	// _init_scanner(tkn, src);

	scanner.src = src;
	scanner.tkns = tkn;

	// pointers to the start of the string
	// and the current character
	scanner.file_start = (char*) src->cptr;
	size len = src->len;
	scanner.line = 1;
	char* line_start = scanner.file_start;
	scanner.cur = src->cptr;

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
				case '(': 
				case ')': 
				case '{': 
				case '}': 
				case '[': 
				case ']': 
				case ';': 
				case ',': 
				case '?': 
				case ':': 
				case '#': 
				case '@': 
				case '^':
					_push_tok(_cur());
				break;
				case '&':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_AND_EQ);
					} else if (_peek() == '&') {
						_advance();
						_push_tok(T_LAND);
					}
					else _push_tok(_cur());
				break;
				case '|':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_OR_EQ);
					} else if (_peek() == '|') {
						_advance();
						_push_tok(T_LOR);
					}
					else _push_tok(_cur());
				break;
				case '.':
					if (_peek() == '.') {
						_advance();
						_push_tok(T_DOT_DOT);
					}
					else _push_tok(_cur());
				break;
				case '*':
					if ( _peek() == '=') {
						_advance();
						_push_tok(T_MUL_EQ);
					}
					else _push_tok(_cur());
				break;
				case '+':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_ADD_EQ);
					} 
					// else if (_peek(c) == '+') {
					// _push_tok(_INC,  (String){.len = 2, .c_ptr = c},c - line_start);
					// 	_advance();
					// }
					else _push_tok(_cur());
				break;
				case '-':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_SUB_EQ);
					} 
					// else if (_peek(c) == '-') {
					// _push_tok(_DEC,  (String){.len = 2, .c_ptr = c},c - line_start);
					// 	_advance();
					// }
					else if (_peek() == '>') {
						_advance();
						_push_tok(T_ARROW);
					}
					else _push_tok(_cur());
				break;
				case '/':
					if (_peek() == '=') { 
						_advance();
						_push_tok(T_DIV_EQ);
					} 
					// COMMENT
					else if  (_peek() == '/') {
						while(_peek() != '\n') {
							_advance();
						}
						line_start = scanner.cur;
					}
					else _push_tok(_cur());
				break;
				case '%':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_MOD_EQ);
					}
					else _push_tok(_cur());
				break;
				case '!':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_NOT_EQ);
					}
					else _push_tok(_cur());
				break;
				case '=':
					if (_peek() == '=')
					{
						_advance();
						_push_tok(T_EQ_EQ);
					} 
					else if (_peek() == '>') {
						_advance();
						_push_tok(T_FAT_ARROW);
					}
					else _push_tok(_cur());
				break;
				case '<':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_LT_EQ);
					}
					else if (_peek() == '<' ) {
						_advance();
						if(_peek() == '=') {
							_advance();
							_push_tok(T_SHL_EQ);
						} else _push_tok(T_SHL);
					}
					else _push_tok(_cur());
				break;
				case '>':
					if (_peek() == '=') {
						_advance();
						_push_tok(T_GT_EQ);
					}
					else if (_peek() == '>' ) {
						_advance();
						if(_peek() == '=') {
							_advance();
							_push_tok(T_SHR_EQ);
						} else _push_tok(T_SHR);
					}
					else _push_tok(_cur());
				break;
				// STRING HANDLING
				case '"':
				{
					if(_peek() == '"') {
						_advance_by(2);
						_push_tok ( T_LIT_STR );
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
					_push_tok(T_LIT_STR);
					break;
				}
				case '\'': {
						_advance();
					if(_peek() != '\'') {
						printf("ERROR, unterminated character\n");
						_advance();
						continue;
					} else _advance();
					_push_tok ( T_LIT_CHAR );
					break;
				}
				default:
					printf("Unknown character encountered: %c at l:%i\n", _cur(), _line());
				break;
			}
		}
	}

	_push_tok(T_EOF);
	return tkn;
}

void print_token_array( dstr *src, dbuf_token tkn) {
	printf("===DUMPING TS===\n");
	// printf("File: %s\n", src.path);
	printf("[TI:CI] token[TN]: tkn, value: val\n"
				 "----------------------------------\n");
	for (int i = 0; i < tkn.current; i++ ) {
		Token current = tkn.data[i];
		switch(current.type) {
			case T_LIT_INT:
			case T_LIT_FLOAT:
			case T_LIT_CHAR:
			case T_LIT_STR:
				printf("[%02i:%02li] source: \"%.*s\" token[%02i]: %s, value: %.*s\n", 
					i, 
					current.span.cptr - src->cptr,
					(int)current.span.len,
					current.span.cptr,
					current.type,
					tok_to_str(current.type), 
					(int)current.span.len,
					current.span.cptr
					);
			break;
			default:
			if(current.type < TKNS_START) {
				printf("[%02i:%02li] source: \"%.*s\" token[%02i]: %c\n", 
					i,
					current.span.cptr - src->cptr,
					(int)current.span.len,
					current.span.cptr,
					current.type,
					current.type
				);	
			} else { printf("[%02i:%02li] source: \"%.*s\" token[%02i]: %s\n", 
					i,
					current.span.cptr - src->cptr,
					(int)current.span.len,
					current.span.cptr,
					current.type,
					tok_to_str(current.type)
				); }
			break;
		}
	}
	printf("Total used tokens: %ld, Unused tokens: %ld\n", tkn.current, tkn.cap - tkn.current);
}

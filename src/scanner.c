#include "scanner.h"
#include "derp.h"

#include <stdio.h>
#include <string.h>

// internal scanner state
struct Scanner {
	int line; // current line number
	dstr src;
	char *cur;        // current character
	char *tok_start;  // start of the current lexeme
	char *file_start;
	char *line_start; // start of the current line
};

#define TOKEN_ERROR (Token) { T_ERROR, .line = scanner.line, .col_index = scanner.cur - scanner.line_start }

const char* tok_to_str(int t) {
	// shitty hack but i can't think of any other solutions
	// to the base-case of single character tokens
	if (t > 0 && t < 128) {
		static char chr[128][2];
		chr[t][0] = (char)t;
		return chr[t];
	}

	switch((TokenTag)t) {
		#define TKN(t, s) case t: return s; break;
			TKNS
		#undef TKN
		case T_NONE:
			return "<none>"; break;
		case TKNS_MAX: case TKNS_START:
			return "<undefined token>"; break;
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
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f');
}

static inline char _peek_by(int by) {
	char *c = scanner.cur + by;
	return (c < scanner.file_start + scanner.src.len) ? *c : '\0';
}

// returns the next character in a string without consuming the current one
static inline char _peek(void) {
	return _peek_by(1);
}

static inline char _cur(void) {
	return _peek_by(0);
}

static inline bool _at_end(void) {
	return scanner.cur >= scanner.file_start == scanner.src.len;
}

// checks if the current + the next n characters match a string
static inline bool _chk_next_n (int n, char chars[]) {
	for(int i = 0; i < n; i++) {
		if(_peek_by(i) != chars[i]) {
			return false;
		}
	}

	return true;
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


static inline bool _is_stringy(void) {
	char c = _cur();
	return ( c == '"' );
}


static Token _make_tok(TokenTag type, TokCat cat) {
	return (Token) {
		type,
		scanner.line,
		scanner.tok_start - scanner.line_start,
		(dstr){scanner.tok_start, scanner.cur - scanner.tok_start + 1},
		cat
	};
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


static Token _scan_num(void) {
	TokenTag tag = T_LIT_INT;

	while(_is_digit(_peek())) {
		_advance();
	} 
	
	if (_peek() == '.' && _is_digit(_peek_by(2) ) ) {
		tag = T_LIT_FLOAT;
		_advance();
		while(_is_digit(_peek())) {
			_advance();
		}
		if(_peek() == 'f' || _peek() == 'd') _advance();
	}
	Token out = _make_tok(tag, TC_LITERAL);
	_advance();
	return out;
}

TokenTag _lookup_keyword(const char* str_ptr, usize str_len) {
	for(int i = T_KEYWORD_START+1; i < T_KEYWORD_END; i++) {
		const char * tokstr = tok_to_str(i);
		if (
		str_len == strlen(tokstr) &&
		     !memcmp(str_ptr, tokstr, str_len)) {
			// keyword detected, yaaay
			return i;
		}
	}
	return T_IDEN;
}

static Token _scan_word(void) {
	// the first letter of an identifier MUST be an alphabetical character, but every subsequent character can be alphanumeric.
	// so _1234 is a valid identifier but 12t is not for example.
	while(_is_alphanum(_peek())) {
		_advance();
	}

	char* str_ptr = scanner.tok_start;
	usize str_len = scanner.cur-scanner.tok_start + 1;
	TokenTag tag = _lookup_keyword(str_ptr, str_len);

	Token out = _make_tok ( tag, tag == T_IDEN ? TC_IDEN : TC_KEYWORD );
	_advance();
	return out;
}

void _skip_whitespace(void) {
	while ( _is_whitespace( _cur() ) ) {
		if ( _cur() == '\n' ) {
			scanner.line++;
			_advance();
			scanner.line_start = scanner.cur;
		} else {
			_advance();
		}
	}
}

static Token _scan_string(void) {
  while ( _peek() != '"' ) {
    if ( _peek() == '\n' || _peek() == '\0' ) {
      dlog_error("Unterminated string at l:%i\n", _line());
      _advance();
      return TOKEN_ERROR;
    }
    if ( _peek() == '\\' ) {
      _advance();                 
      if ( _peek() == '\0' ) {
        dlog_error("Unterminated string at l:%i\n", _line());
	      _advance();
        return TOKEN_ERROR;
      }
    }
    _advance();
  }

  _advance();

  Token out = _make_tok(T_LIT_STR, TC_LITERAL);
  _advance();

  return out;
}

void _skip_comments(void) {
	if( _chk_next_n(2, "//") ) {
		_advance_by(2);
		while(_cur() != '\n' && !_at_end()) _advance();
	}

	if( _chk_next_n(2, "/*") ) {
		_advance_by(2);
		int depth = 1;
		
		while ( depth > 0 && !_at_end() ) {
			if(_cur() == '\n') {
				scanner.line ++;
				_advance();
				scanner.line_start = scanner.cur;
			} else if ( _chk_next_n(2, "/*") ) {
				depth++;
				_advance_by(2);
			} else if ( _chk_next_n(2, "*/") ) {
				depth--;
				_advance_by(2);
			}
			else {
				_advance();
			}
		}

		if ( depth > 0 ) {
			// idk throw an error here
			// for now i'm just going to panic
			// FIXME: add actual error handling here.
			dlog_error("Unterminated multiline comment :(");
			exit(-1);
		}
	}
}


void scanner_init(dstr src) {
	scanner.src = src;
	scanner.line = 0;
	scanner.cur = src.cptr;
	scanner.file_start = src.cptr;
	scanner.tok_start = src.cptr;
	scanner.line_start = src.cptr;
}

// convert input string into list of tokens
Token next_token(void) {
	while (true) {
    char *before = scanner.cur;
    _skip_whitespace();
    _skip_comments();
    if (scanner.cur == before) break;
	}

	scanner.tok_start = scanner.cur;

	Token out;

	if      ( _is_digit(_cur()) ) { return _scan_num(); }
	else if ( _is_alpha(_cur()) ) { return _scan_word(); }
	else if ( _is_stringy() )     { return _scan_string(); }

	else {
		switch ( _cur() ) {
			case '(':  case ')': 
			case '{':  case '}': 
			case '[':  case ']': 
			case ';':  case ',': 
			case '?':  case '#': 
			case '@': 
				out = _make_tok( (TokenTag)_cur(), TC_SYMBOL);
			break;

			// ^ ^=
			case '^':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_BIXOR_EQ, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// : :=
			case ':':
				if(_peek() == '=') {
					_advance();
					out = _make_tok(T_ASSIGN, TC_OPERATOR);
				} else {
					out = _make_tok(_cur(), TC_SYMBOL); 
				}
			break;

			// & && &=
			case '&':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_BAND_EQ, TC_OPERATOR);
				} else if (_peek() == '&') {
					_advance();
					out = _make_tok(T_LAND, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// | || |=
			case '|':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_BOR_EQ, TC_OPERATOR);
				} else if (_peek() == '|') {
					_advance();
					out = _make_tok(T_LOR, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// . .. ...
			case '.':
				if (_peek() == '.') {
					_advance();
					if(_peek() == '.') {
						_advance();
						out = _make_tok(T_ELLIPSE, TC_OPERATOR);
					}
					else {
							out = _make_tok(T_DOT_DOT, TC_OPERATOR);
						}
				}
				else {
						out = _make_tok(_cur(), TC_OPERATOR);
					}
			break;

			// * *=
			case '*':
				if ( _peek() == '=' ) {
					_advance();
					out = _make_tok(T_MUL_EQ, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// + +=
			case '+':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_ADD_EQ, TC_OPERATOR);
				} 
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// - -= ->
			case '-':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_SUB_EQ, TC_OPERATOR);
				} 
				// else if (_peek(c) == '-') {
				// out = _make_tok(_DEC,  (String){.len = 2, .c_ptr = c},c - line_start);
				// 	_advance();
				// }
				else if (_peek() == '>') {
					_advance();
					out = _make_tok(T_ARROW, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// / /=
			case '/':
				if (_peek() == '=') { 
					_advance();
					out = _make_tok(T_DIV_EQ, TC_OPERATOR);
				} 
				// COMMENT
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// % %=
			case '%':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_MOD_EQ, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// ! !=
			case '!':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_NOT_EQ, TC_OPERATOR);
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// == =>
			case '=':
				if (_peek() == '=')
				{
					_advance();
					out = _make_tok(T_EQ_EQ, TC_OPERATOR);
				} 
				else if (_peek() == '>') {
					_advance();
					out = _make_tok(T_FAT_ARROW, TC_OPERATOR);
				} else {
					goto error_case;
				}
			break;

			// < <= << <<=
			case '<':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_LT_EQ, TC_OPERATOR);
				}
				else if (_peek() == '<' ) {
					_advance();
					if(_peek() == '=') {
						_advance();
						out = _make_tok(T_SHL_EQ, TC_OPERATOR);
					} else {
						out = _make_tok(T_SHL, TC_OPERATOR);
					}
				}
				else {
						out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			// > >= >> >>=
			case '>':
				if (_peek() == '=') {
					_advance();
					out = _make_tok(T_GT_EQ, TC_OPERATOR);
				}
				else if (_peek() == '>' ) {
					_advance();
					if(_peek() == '=') {
						_advance();
						out = _make_tok(T_SHR_EQ, TC_OPERATOR);
					} else {
						out = _make_tok(T_SHR, TC_OPERATOR);
					}
				}
				else {
					out = _make_tok(_cur(), TC_OPERATOR);
				}
			break;

			default:
				if( _at_end() ) {
					return TOKEN_EOF;
				}
				error_case:
				dlog_error("Unknown character encountered: '%c'(%i) at l:%i\n",
          _cur(),
          _cur(),
          _line()
        );
				_advance();
				return TOKEN_ERROR;
			break;
		}

		_advance();
		return out;
	}
}

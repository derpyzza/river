#include "parser.h"
#include "parser-internal.h"

#include "../diagnostics.h"
// #include "scanner.h"

Token cur_tok(void) {
	return parser.cur; 
}

Token prev_tok(void) {
	return parser.prev;
}

Token expect(int tag, const char* msg) {
	if (cur_tok().type == tag) {
		return consume();
	}

	error_expected(cur_tok(), tag, true, msg);

	return TOKEN_NONE;
}

void push_error(ParseError e) {
  parser.had_error = true;
  dbuf_ParseErrors_push(&parser.errors, e);
}

void error_expected(Token got, int expected, bool is_tok, const char* msg){
	if(parser.panic_mode) return;
	parser.panic_mode = true;
	
	ParseError err = {0};
	err.kind = is_tok ? PERR_EXPECT_TOK : PERR_EXPECT_CAT;
	err.expected = expected;
	err.got = got;
	err.msg = msg;
	// err->debug_file = file;
	// err->debug_line = line;
	push_error(err);
}

void error_at(Token at, const char* msg) {
  if (parser.panic_mode) return;
  parser.panic_mode = true;
  push_error((ParseError){
    .kind = PERR_MSG, .got = at, .msg = msg,
  });
}

void synchronize(void) {
  parser.panic_mode = false;

  while (parser.cur.type != T_EOF) {
    if (parser.prev.type == ';') return;

    switch (parser.cur.type) {
      case T_IMPORT: case T_STRUCT: case T_FUN:
      case T_LET: case T_IF: case T_WHILE:
      case T_FOR: case T_RETURN:
        return;
      default: break;
    }
    consume();
  }
}

// void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line) {
// 	error_unexpected(parser.current + 1, expected, is_expected_tok, file, line);
// 	if (is_expected_tok)
// 		while(!check_next(T_EOF) && !check_next(until)) consume();
// 	else
// 		while(!check_next(T_EOF) && !check_next_cat(until)) consume();
// }

void print_source_line(Token t) {
  const char *begin = parser.source.cptr;
  const char *end   = begin + parser.source.len;
  const char *p     = t.span.cptr;

  // synthetic token (e.g. EOF)
  if (p < begin || p > end) {
  	return;
  }

  const char *ls = p;
  const char *le = p;
  while (ls > begin && ls[-1] != '\n') { ls--; }
  while (le < end   && *le    != '\n') { le++; }

  int pad  = (int)(p - ls) + 2;
  int span = t.span.len > 0 ? (int)t.span.len : 1;

  fprintf(stderr, "%li | %.*s\n  %*s", t.line, (int)(le - ls), ls, pad, "");
  for (int i = 0; i < span; i++) fputc('^', stderr);
  fputc('\n', stderr);
}

void report_error(ParseError *e) {
// #	ifdef DEBUG
// 	printf("[%sCOMPILER_DEBUG%s]: error emitted from <%s:%i>\n", 
// 	      TERM_LRED,
//        	TERM_RESET, 
// 				error->debug_file,
// 				error->debug_line);
// # endif

	fprintf(stderr, "%.*s:%i:%i: ERROR ",
		dstr_fmt(parser.file_name),
		(int)e->got.line,
		(int)e->got.col_index
	);
	switch(e->kind) {
		case PERR_EXPECT_TOK:
			fprintf(stderr, "%s (expected '%s', got '%.*s' <%s>)\n",
			  e->msg ? e->msg : "Unexpected token",
				tok_to_str((TokenTag)e->expected),
				dstr_fmt(e->got.span),
				tok_to_str(e->got.type)
			);
		break;
		case PERR_EXPECT_CAT:
			fprintf(stderr, "%s (expected <%s>, got '%.*s' <%s>)\n",
		  e->msg ? e->msg : "Unexpected token",
			cat_to_str((TokCat)e->expected),
			dstr_fmt(e->got.span),
			tok_to_str(e->got.type)
		);
		break;
		case PERR_LEX:
			fprintf(stderr, "%s %.*s \n",
			  e->msg ? e->msg : "Invalid token",
				dstr_fmt(e->got.span)
			);
		break;
		case PERR_MSG:
      fprintf(stderr, "%s\n", e->msg);
		break;
	}
	print_source_line(e->got);
}

void report_errors(void) {
	for (int i = 0; i < parser.errors.cur; i++) {
		report_error(&parser.errors.data[i]);
	}
}

Token consume(void) {
	if(parser.cur.type != T_EOF) {
		parser.prev = parser.cur;
		while (true) {
			parser.cur = next_token();
			if(parser.cur.type != T_ERROR) break;
			if(!parser.panic_mode) {
				push_error((ParseError){
					.kind = PERR_LEX, .got = parser.cur, .msg = "Invalid Token"
				});
				parser.panic_mode = true;
			}
		}
	}
	return parser.prev;
}

bool match( TokenTag expected ) {
	if ( parser.cur.type != expected ) { return false; }

	consume();
	return true;
}

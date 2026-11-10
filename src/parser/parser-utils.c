#include "parser.h"
#include "parser-internal.h"

#include "../diagnostics.h"

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
	// FIXME: get proper error handling, reporting, and synchronization in here
	dlog_error("%s at file %.*s, line #%lu: %.*s", msg, dstr_fmt(parser.file_name), cur_tok().line, dstr_fmt(cur_tok().span));
	// FIXME: get proper line and file reporting in here
	error_unexpected(cur_tok(), tag, true, "DEBUG_FILE", 2048);
	return TOKEN_NONE;
	exit(-1);
}

void error_unexpected( Token got, int expected, bool is_tok, const char * file, const int line ){
	if (!parser.had_error) {
		parser.had_error = true;
		parser.errors = dbuf_ParseErrors_make(8);
	}
	struct ParseError *err = (ParseError*)malloc(sizeof(struct ParseError));
	err->expected = expected;
	err->got = got;
	err->debug_file = file;
	err->debug_line = line;
	err->is_tok = is_tok;
	dbuf_ParseErrors_push(&parser.errors, *err);
}

// void panic(int expected, int is_expected_tok, int until, int is_until_tok, const char * file, const int line) {
// 	error_unexpected(parser.current + 1, expected, is_expected_tok, file, line);
// 	if (is_expected_tok)
// 		while(!check_next(T_EOF) && !check_next(until)) consume();
// 	else
// 		while(!check_next(T_EOF) && !check_next_cat(until)) consume();
// }

void report_error(ParseError *error) {
	printf("[%sCOMPILER_DEBUG%s]: error emitted from <%s:%i>\n", 
	      TERM_LRED,
       	TERM_RESET, 
				error->debug_file,
				error->debug_line);

	if ( error->is_tok ) {
		printf("%i:%i: ERROR Unexpected token; expected '%s', got '%.*s' (%s)\n",
		// printf("%s:%i:%i: ERROR Unexpected token; expected '%s', got '%.*s' (%s)\n",
			// parser.source->path,
			(int)error->got.line,
			error->got.col_index,
			tok_to_str((TokenTag)error->expected),
			dstr_fmt(error->got.span),
			tok_to_str(error->got.type)
		);
	} else {
		// FIXME: there's a bug in here where error_unexpected expects `got` to be
		// a `Token` but there's no `Token` variant of `TokCat`.
		printf("%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
		// printf("%s:%i:%i: ERROR Unexpected token; expected <%s>, got '%.*s' (%s)\n",
			// parser.source->path,
			(int)error->got.line,
			error->got.col_index,
			cat_to_str((TokCat)error->expected),
			dstr_fmt(error->got.span),
			tok_to_str(error->got.type)
		);
	}
}

bool check_errors(void) {
	if (parser.had_error) {
		for (int i = 0; i < parser.errors.cur; i++) {
			report_error(&parser.errors.data[i]);
		}
		return true;
	} else return false;
}

// int match_cat(int cat) {
// 	if ( tok_to_cat(peek().type) == cat ) { 
// 		consume();
// 		return 1; 
// 	} 
// 	return 0;
// }

Token consume(void) {
	if(parser.cur.type != T_EOF) {
		parser.prev = parser.cur;
		while (true) {
			parser.cur = next_token();
			if(parser.cur.type != T_ERROR) break;
			error_unexpected(parser.cur, ' ', false, "", 0);
		}
	}
	return parser.prev;
}

bool match( TokenTag expected ) {
	if ( parser.cur.type != expected ) { return false; }

	consume();
	return true;
}

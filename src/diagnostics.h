#pragma once

#define TERM_BOLD = "\x1B[1m";
#define TERM_ITALICIZE = "\x1B[3m";
#define TERM_UNDERLINE = "\x1B[4m";

static const char* TERM_RESET  = "\x1B[0m";
static const char* TERM_LBLACK  = "\x1B[30m";
static const char* TERM_LRED  = "\x1B[31m";
#define TERM_LGREEN  "\x1B[32m"
#define TERM_LYELLOW  "\x1B[33m"
#define TERM_LBLUE  "\x1B[34m"
#define TERM_LMAGENTA  "\x1B[35m"
#define TERM_LCYAN  "\x1B[36m"
#define TERM_LWHITE  "\x1B[37m"

enum DiagLevel {
  D_ERROR,
  D_WARN,
  D_HINT
};

union Error {
  // do something.
  struct ParseError *parse;
};

// static inline void report_parse_error(struct ParseError *err) {
  
// }

// static inline void report_error(union Error *err) {

// }

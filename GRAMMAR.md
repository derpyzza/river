program -> statement* EOF;

program -> declaration*;

declaration -> func_decl* | global_var_decl* | ("static"? | "const"?) declaration;

global_var_decl -> (("static" ("mut")?) | ("const"))? TYPE ID = LITERAL ";";

func_decl -> "pub"? TYPE ID ( "(" ( TYPE ID ("," TYPE ID)* )? ")" )? "=>" block
block -> "{" expr* "}";

var_decl -> (TYPE | "let") "mut"? ID ( ";" | "=" LITERAL ";"); 

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false";

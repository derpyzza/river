
program -> Item* EOF;

Item ->
      import_decl
    | struct_decl
    | type_def
    | enum_decl
    | union_decl
    | fn_def
    | global_var;

import_decl -> "import" ID(.ID)*? ("as" ID)? ";"

struct_decl -> "struct" ID "{" ( TYPE ID ( "=" LIT )? ";" ); "}"

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

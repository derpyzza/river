Language Grammar
===

the grammar of river. currently a mess but will get better with time.

program -> Item* EOF;

Item ->
      import_decl
    | struct_decl
    | type_def
    | enum_decl
    | union_decl
    | fn_def
    | global_var;

TYPE -> BUILTIN_TYPES | ID;

import_decl -> "import" ID(.ID)*? ("as" ID)? ";";

func_decl -> "pub"? TYPE ID ( "(" params_list ")" )? ( "=>" expr | block_expr );

params_list -> param_item? ( "," param_item )*;
param_item -> TYPE ID ( "=" LIT )?


global_var -> TYPE var_assign ( "," var_assign )*;

var_assign -> ID '=' LIT;

let type [VAL];
VAL -> ( ID | (ID = LIT)) ","*

struct_decl -> "struct" ID "{" ( TYPE ID ( "=" LIT )? ";" ); "}"

global_var_decl -> (("static" ("mut")?) | ("const"))? TYPE ID = LITERAL ";";

block -> "{" expr* "}";

var_decl -> (TYPE | "let") "mut"? ID ( ";" | "=" LITERAL ";"); 

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false";

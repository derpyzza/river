Language Grammar
===

the grammar of river. currently a mess but will get better with time.

program -> item* EOF;

item -> import_decl
      | const_decl;
      | global_var
      | struct_decl
      | fn_def
      | type_def
      | enum_decl
      | union_decl;

TYPE -> BUILTIN_TYPES | ID;

import_decl -> "import" ID(.ID)*? ("as" ID)? ";";

func_decl -> "pub"? TYPE ID ( "(" params_list ")" )? ( "=>" expr | block_expr );

params_list -> param_item? ( "," param_item )*;
param_item -> TYPE ID ( "=" LIT )?


global_var -> TYPE var_assign ( "," var_assign )*;
var_assign -> ID '=' LIT;

// constexpr
const_decl -> "const" ID "=" expr;

struct_decl -> "struct" ID "{" (struct_field ";")+ "}";

struct_field -> TYPE struct_item ("," struct_item)*

struct_item -> ID ("=" LIT)?

block -> "{" expr* "}";

var_decl -> (TYPE | "let") "mut"? ID ( ";" | "=" LITERAL ";"); 

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false";

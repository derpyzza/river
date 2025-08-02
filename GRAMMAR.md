Language Grammar
===
the grammar of river. currently a mess but will get better with time.

# Notation:

rules are in snake_case, and are defined with the following pattern:

```
rule -> production | ( production | production );
```

the `|` character means `or`, and rules can be grouped via parenthesis for clarity.
rules are terminated with the `;` character.

terminals are wrapped in quotes `"import"`.

production rules can be followed up with any of the following operators:
```
*: Needs zero or more of
+: Needs one or more of
?: optionally can have
```

```

iden -> [a-zA-Z_]+ ;
number -> int | float ;


program -> item* eof;

item -> import_decl
      | const_decl;
      | global_var
      | struct_decl
      | fn_def
      | type_def
      | enum_decl
      | union_decl;

import_decl -> "import" iden ( "." iden )*? ( "as" iden )? ";" ;

func_decl -> "pub"? "fun" iden ( "(" params_list ")" )? ( ":" type )? "=" expr;

params_list -> param_item? ( "," param_item )*;
param_item -> iden ( ":" iden )? ( "=" lit )?

data_type -> <primary_type> ( "," <primary_type> | "|" <primary_type> )* ;
primary_type -> <type_header> iden ;
type_header -> "^"* type_arr* ;
type_arr -> ("[" %number? "]")* "^"* ;

global_var -> iden <var_assign> ( "," <var_assign> )*;
var_assign -> iden '=' %lit;

const_decl -> "const" iden "=" <expr>;

type_def ->
        "type" iden "=" basic_type ";"
      | "type" iden "=" (struct_decl | union_decl | enum_decl)
      ;

struct_decl -> "struct" "{" ( struct_field "," )+ "}";
struct_field -> iden ("," iden)* ":" type string? ;

union_decl -> "raw"? "union" "{" ( union_field "," )+ "}";
union_field -> iden ":" type string? ;
enum_decl -> "enum" ( ":" (iden ',')* iden ) "{" (iden ",")* "}";

var_decl -> "let" "mut"? iden ( ":" type )? ( ";" | "=" LITERAL ";"); 


type -> ptr* arr iden
      | iden ( ',' iden )*
      ;

statement -> expr ";";

stmt -> expr_stmt
      | decl_stmt;

expr_stmt -> expr ";" ;
decl_stmt -> ( 'let' | TYPE ) ID "=" expr;

block_expr -> "{" (( stmt* expr? ) | expr) "}" ; 

expr -> "{" ( assignment ";"? | assignment )+ "}" | assignment;
assignment -> ID "=" assignment | lor ;

lor -> land ( "||" land )* ;
land -> eq ( "&&" eq )* ;
eq -> comp ( ( "!=" | "==" ) comp )* ;
comp -> shift ( ( ">" | ">=" | "<" | "<=" ) shift )* ;
shift -> add ( (">>" | "<<") add)*

add -> mult ( ('+'|'-') mult )*;
mult -> unary ( ('*' | '/' | '%') unary)*;
unary -> primary | ( ('-' | '!') primary);

primary 
      -> "(" expr ")"
      | INT 
      | FLOAT
      | STRING
      | CHAR
      | ARRAY_LIT
      | "true" 
      | "false" 
      | TUPLE_LIT 
      | STRUCT_LIT;

```

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

tokens are marked with the `%` character, terminals are wrapped in quotes `"import"`.

production rules can be followed up with any of the following operators:
```
*: Needs zero or more of
+: Needs one or more of
?: optionally can have
```

```

let x: ^^[^^int ; 2 ; 4] | ^^^[^file, error ; 23] | ^char;

type -> ptr* arr %id
      | %id ( ',' %id )*
      | %id ( '|' %id )*

program -> item* %eof;

item -> import_decl
      | const_decl;
      | global_var
      | struct_decl
      | fn_def
      | type_def
      | enum_decl
      | union_decl;

import_decl -> "import" %id ( "." %id )*? ( "as" %id )? ";" ;

func_decl -> "pub"? "fun" %id ( "(" <params_list> ")" )? ( "->" %id )? "=" <expr>;

params_list -> <param_item>? ( "," <param_item> )*;
param_item -> %id ( ":" %id )? ( "=" %lit )?

data_type -> <primary_type> ( "," <primary_type> | "|" <primary_type> )* ;
primary_type -> <type_header> %id ;
type_header -> "^"* type_arr* ;
type_arr -> ("[" %number? "]")* "^"* ;

global_var -> %id <var_assign> ( "," <var_assign> )*;
var_assign -> %id '=' %lit;

const_decl -> "const" %id "=" <expr>;

type_def = "type" %id "=" <expr> ";" ;

struct_decl -> "struct" %id "{" ( <struct_field> ";" )+ "}";
struct_field -> struct_item ("," struct_item)* ":" %id ;
struct_item -> %id ("=" %lit)?

var_decl -> "let" "mut"? %id ( ":" %id )? ( ";" | "=" LITERAL ";"); 

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

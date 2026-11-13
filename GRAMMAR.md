Language Grammar
===
the grammar of river. currently a mess but will get better with time.

# Notation:

rules are in snake_case, and are defined with the following pattern:

```
rule:
      production
      | ( production | production )
```

the `|` character means `or`, and rules can be grouped via parenthesis for clarity.

terminals are wrapped in quotes `"import"`.

production rules can be followed up with any of the following operators:
```
*: Needs zero or more of
+: Needs one or more of
?: Either one or zero of
```

```

iden:
       [a-zA-Z_]+

number:
      int | float 


program ->
      item* eof

vis ->
      "pub"

item ->
      | import_decl
      | vis? (
            | const_decl
            | var_decl
            | func_decl
            | type_decl
            | struct_decl
            | enum_decl
            | union_decl
            | trait_decl
            | impl_decl
      )


import_decl ->
      "import" iden ( "." iden )* ( "as" iden )? ";"


const_decl ->
      "const" iden ":" type "=" expr ";"


var_decl ->
          | ("let" | "var") iden ( "," iden )* ":" type ";"
          | ("let" | "var") iden ( "," iden )* ( ":" type )? "=" expr ";"


func_decl ->
      | fun iden "(" params_list? ")" ( ":" type )? ';'
      | fun iden "(" params_list? ")" ( ":" type )? block_expr
      | fun iden "(" params_list? ")" ( ":" type )? "=" blockless_expr ';'


params_list ->
      param_item ( "," param_item )*;

param_item ->
      | iden ":" type ( "=" lit )?
      | iden ( "," iden )+ ":" type


type_def ->
      "type" iden "=" data_type ";"


struct_decl ->
      "struct" "{" ( struct_field ";" )+ "}"


struct_field ->
      | iden ":" type ( "=" lit )? string?
      | iden ( "," iden )+ ":" type string? 


union_decl ->
      "raw"? "union" "{" ( union_field "," )+ "}"


union_field ->
      iden ":" type string?


enum_decl ->
      "enum" ( ":" (iden ',')* iden )? "{" (iden ",")* "}"


type ->
      | ptr* arr iden
      | iden ( ',' iden )*


stmt ->
      (
            | expr
            | var_decl
            | const_decl
            | loop_stmt     
            | "return" expr
            | "break"
            | "continue"
      ) ";"


expr ->
      | block_expr
      | blockless_expr

      
block_expr ->
      "{" (( stmt+ expr? ) | expr) "}"

blockless_expr ->
      "{" ( assignment ";"? | assignment )+ "}" | assignment

assignment ->
      | iden "=" assignment
      | lor 

lor ->
      land ( "||" land )*
land ->
      eq ( "&&" eq )*
eq ->
      comp ( ( "!=" | "==" ) comp )*
comp ->
      shift ( ( ">" | ">=" | "<" | "<=" ) shift )*
shift ->
      add ( (">>" | "<<") add)*

add ->
      mult ( ('+'|'-') mult )*
mult ->
      unary ( ('*' | '/' | '%') unary)*
unary ->
      | primary
      | ( ('-' | '!') primary)

primary ->
      | "(" blockless_expr ")"
      | int_lit 
      | float_lit
      | string_lit
      | char_lit
      | array_lit
      | tuple_lit 
      | struct_lit
      | "true" 
      | "false" 

type ->
      type_header iden
type_header ->
      "*"* type_arr*
type_arr ->
      ("[" %number? "]")* "^"*

```

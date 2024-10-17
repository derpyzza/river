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

Constants are in uppercase:
TYPE -> a type
ID -> identifier
INTEGER -> an integer literal
etc.

keywords are wrapped in quotes: `"if"`, `"for"`, `"import"`;

production rules can be followed up with any of the following operators:
```
*: Needs zero or more of
+: Needs one or more of
?: optionally can have
```

```
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

const_decl -> "const" ID "=" expr;

struct_decl -> "struct" ID "{" (struct_field ";")+ "}";
struct_field -> TYPE struct_item ("," struct_item)*
struct_item -> ID ("=" LIT)?

block -> "{" expr* "}";

var_decl -> (TYPE | "let") "mut"? ID ( ";" | "=" LITERAL ";"); 

statement -> expr ";";

expr -> add ( ('+'|'-') add )*;
add -> mult ( ('\*' | '\/') mult)*;
mult -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false" | TUPLE;
```

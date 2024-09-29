program -> statement* EOF;

assignment -> type iden "=" expr ";";

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false";

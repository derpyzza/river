program -> statement* EOF;

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT;

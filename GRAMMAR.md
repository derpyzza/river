program -> statement* EOF;

program -> declaration*;

declaration -> func;

func -> "int main" "( )" block;
block -> "{" expr "}";

statement -> expr ";";

expr -> term ( ('+'|'-') term )*;
term -> factor ( ('*' | '/') factor)*;
factor -> unary | ( '-' | '!' unary);
unary -> primary;
primary -> INT | FLOAT | DOUBLE | "true" | "false";

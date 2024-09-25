step one:
expression -> INT | ( INT '+' INT ) EOF

---

statement -> expression ";"

expression -> term ( ( '+' | '-' ) term )*;

term -> factor ( ( '*' | '-' ) factor )*;

factor -> INT;

# Introduction
This is the language spec for river.

## Character / operator list:
+ - * / % ! != += -= *= /= %= ^ ~ & | && ||
. , > >= <= < ? : ; ' " ` = == ( ) { } [ ]

## Keyword list 

C keywords:

short char int long float double void
signed unsigned 
if else for while break continue
switch case goto struct return 
enum union typedef sizeof const

river keywords:

usize isize let string bool array
i8 i16 i32 i64 u8 u16 u32 u64 f32 f64
foreach defer match label
public private sticky
new delete namespace macro


how about smarter comments?
tagged comments
like @todo @error @bug @note, which the compiler could build notes out of and then store in a file somewhere?


## Variable Declaration

```c

int x = 0;
let x = 45;
x = "str"; // illegal

```

## Statements and Expressions

Expressions are strings of code that evaluate to a value,
whereas statements produce a side-effect;

```c 

// Expressions

x = 3 // outputs 3
"string literal"
12424.41 // Number literal
(object){.x = 2314, .wr = "saftq"} // Struct literal
{
    return x;
} // This whole block is an expression
let x = if (bool) 10 else if (other_bool) 11 else 23; // this is an expression wrapped up in a statement;


// Statements

```


## Control flow

```c 
if ( expression ) {
    // do stuff
} else if ( other_expression ) {
    // do other stuff
} else {
    // do yet more stuff
}

while(expression) {
    // do stuff
}

do {
    // do stuff 
} while(expression);

for (init; condition; step) {

}

foreach ( item, iterable ) {

}

```

## Returns 

```c 

char return_char() {
    char c = 'g';

    c
}


```

## Pointers

```c

// Function Pointers
func* void(int x, float y);

```

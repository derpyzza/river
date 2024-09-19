# Introduction
This is the language spec for river.

## Character / operator list:
+ - * / % ! != += -= *= /= %= ^ ~ & | && ||
. , > >= <= < ? : ; ' " ` = == ( ) { } [ ]

## Keyword list 
let short char int long string float double void bool array
signed unsigned usize isize
i8 i16 i32 i64 u8 u16 u32 u64 f32 f64
if else for while foreach defer break continue
switch case match goto label
struct return enum union typedef sizeof
public private sticky const
new delete namespace


## Variable Declaration

```c
int x = 0;
let x = 45;
x = "str"; // illegal
```

## Statements and Expressions

Statements are ended by a semicolon
Expressions are not.

```c 



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

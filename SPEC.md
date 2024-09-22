# Introduction
This is the language spec for river.

## comments
```c 

// Double slashes indicate a single line comment 

/* 
    A Slash followed by an asterisk marks a multiline comment, 
    which is ended by an asterisk followed by a slash.
    Just like in C!
*/

```

## Variable Definition & Declaration

```c

// The let keyword inferences the type of a variable by whatever the first value is that the 
// variable is assigned to.
// Here it takes on the value of an int.
let x = 45;
x = "str"; // illegal

// Currently the variable y simply has a none type;
let y;

// From this point on, y is now a floating point number;
y = 13314.3f;

// you can also explicitly define a variable with a type.
i32 t = 124;

// The builtin type literals are as follows:
/*
 * u8  - Unsigned 8 bit integer
 * u16 - Unsigned 16 bit integer
 * u32 - Unsigned 32 bit integer
 * u64 - Unsigned 64 bit integer
 * i8  - Signed 8 bit integer
 * i16 - Signed 16 bit integer
 * i32 - Signed 32 bit integer
 * i64 - Signed 64 bit integer
 * f32 - Floating point number
 * f64 - Double precision floating point number
 * bool - boolean type
 * char - A Single utf-8 code point i.e represents a single utf-8 character, and is not a byte like in C.
 * str -  A statically sized utf-8 string literal
 * string - A dynamically sized utf-8 string literal;
*/

```

## structs 
```c

// You can wrap up several datatypes into a big datatype called a struct, just like in C 
struct Vector3 {
    f32 x, y, z;
};

// Unlike in C, you don't need to typedef structs to avoid the struct keyword in declarations
Vector3 v = (Vector3){10.f, 10.f, 12.f};

// However, it's good practice to mark structs with an "_s" when declaring them
struct Vector3_s {
    u32 x, y, z;
};

// Plus, declaring struct variables with the <struct identifier> type signature is also an option
struct Vector3_s v = (Vector3_s){23, 25, 356}

```

## Namespaces
```c

// the namespace keyword declares a new namespace
namespace Vector3 {
    Vector3_s add(Vector3_s x, Vector3_s y);
    Vector3_s sub(Vector3_s x, Vector3_s y);
}

// using a function from a namespace
Vector3::add();
// or
// declaring an automatic namespace
{
    using namespace::Vector3;

    add(x, y); // Vector3::add();
    sub(y, x); // Vector3::sub();
}

```

## Interfaces 
```c 

// Interfaces define polymorphic functions
interface debug {
    string to_string(T t);
    void debug_print(T t);
}

struct Vector3_s {
    f32 x, y, z;
}

impl Vector3_s::debug {
    
    string to_string(Vector3_s t) {
        string x = new string();
        x.concat(`x: {f32::to_string(t.x)}, `);
        x.concat(`y: {f32::to_string(t.y)}, `);
        x.concat(`z: {f32::to_string(t.z)}, `);
        x.concat(`\n`);
        return x;
    }
}

Vector3_s v = (Vector3_s){130, 24, 53};
printf("%s", Vector3_s::to_string(v)); // prints "x: 130, y: 24, z: 53"

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


## strings

## Character / operator list:
+ - * / % ! != += -= *= /= %= ^ ~ & | && ||
. , > >= <= < ? : ; ' " """ ` = == ( ) { } [ ]
@

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

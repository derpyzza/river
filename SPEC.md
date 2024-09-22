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

## Data types and literals
```c 

// River comes with several builtin basic data types
// The builtin type literals are as follows:
/*
 * null     - No type / no value
 * bool     - boolean type / true | false
 * u8       - Unsigned 8 bit integer
 * u16      - Unsigned 16 bit integer
 * u32      - Unsigned 32 bit integer
 * u64      - Unsigned 64 bit integer
 * u128     - Unsigned 128 bit integer
 * i8       - Signed 8 bit integer
 * i16      - Signed 16 bit integer
 * i32      - Signed 32 bit integer
 * i64      - Signed 64 bit integer
 * i128     - Signed 128 bit integer
 * f32      - Floating point number
 * f64      - Double precision floating point number
 * isize    - Signed pointer sized integer
 * usize    - Unsigned pointer sized integer
 * char     - A Single utf-8 code point i.e represents a single utf-8 character, and is not a byte like in C.
 * str      -  A statically sized utf-8 string literal
 * string   - A dynamically sized utf-8 string literal;
*/

// Numeric type literals can have underscores in-between as a visual separator to help with readability
i32 million = 1_000_000; // Same as 1000000

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

```

## types

```c

// You can define your own types

type int -> u32;

int x = 10; // base type: u32

// Types can be composed of multiple base times
// this type can represent both u32 or f32 values
type number -> u32 | f32;

number x = 10; // valid
number y = 52.32; // also valid
number z = 42.4d; // not valid, as it is a double value


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

struct Vector4 {
    // Fields are non-nullable by default
    f32 x, y, z;
    // However, fields can be nullable
    // appending a ? to a variable name makes it nullable
    // Like pointers, nullability is a property of a variable, not a datatype
    f32 w?;
};

struct User {
    // Struct fields can have default values
    str username = "Admin";
    u16 pin = 0000;
};

User admin;
printf("user.name = %s", admin.username); // => "user.name = Admin"
admin.name = "Administrator";
printf("user.name = %s, user.pin = %i", admin.username, admin.pin) // => "user.name = Administrator, user.pin = 0000"; 

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
\+ \- \* / % ! != += -= *= /= %= ^ ~ & | && ||
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

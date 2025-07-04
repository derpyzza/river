# Introduction
This is the language spec for river.
for now this is more of a sketchpad for features, but it'll get leaner and cleaner with each commit!


design patterns:

> ':' attributes
> '=' assigns
> '@' is a built-in procedure
> '#' annotates


# Identifiers and Numbers

```
    // identifier
    %iden -> [a-zA-Z_] [a-zA-Z0-9_-?!%]* ;

    // number literal
    %number -> float | integer;

    // decimal exponent
    dec_exp -> [eE] [-+]? integer ;
    // binary exponent
    bin_exp -> [pP] [-+]? integer ;

    float -> 
        ( 
            \d* ( '.' \d+ )? dec_exp? [fFdD]?
            | hex_literal* ( '.' hex_literal+ ) bin_exp? [fFdD]? 
            | hex_literal* bin_exp? [fFdD]
            ) 
    ;

    integer ->   
        (
            \d* ( ['_] \d+ ) dec_exp?
            | <hex_literal> bin_exp?
        		| <octal_literal>
        		| <binary_literal>
        )
        ( [uUiI] ( \d* | 'z' ) )?
    ;

    binary_literal -> '0b' binary_digit* ( [_'] binary_digit+ )? ;
    hex_literal -> '0x' hex_digit* ( [_'] hex_digit+ )? ;
    octal_literal -> '0c' octal_digit* ( [_'] octal_digit+ )? ;

    hex_digit -> [0-9a-fA-F] ;
    octal_digit -> [0-7] ;
    binary_digit -> [01] ;

    1
    32
    134'43e+10
    134_43E+10
    234f
    13'234'32
    24_242_24
    123.134e+13

    0xbadcafe
    0xBADCAFE
    0xBad_Cafe
    0xBAD'CAFE
    0xBad.Cafe
    0xBAD.CAFEp+24

    0c777
    0c11_23
    0c32'24'24

    0b101010101
    0b1010'1010
    0b1001_1010

    123.234
    12.23_24
    .05
    123'234.23'234
    142_234.244_242
    123u8      // explicit u8 literal
    123u16     // explicit u16 literal
    123u32     // explicit u32 literal
    123u64     // explicit u64 literal
    123i8      // explicit i8 literal
    123i16     // explicit i16 literal
    123i32     // explicit i32 literal
    123i64     // explicit i64 literal
    234uz      // explicit usize literal
    424iz      // explicit size literal
    133f32     // explicit f32 literal
    234f64     // explicit f64 literal
```

# Comments
```rs 

// Double slashes indicate a single line comment 

/*
    A Slash followed by an asterisk marks a multiline comment, 
    which is ended by an asterisk followed by a slash.
    Just like in C!

    /*
        however, unlike in C, comments can be nested
    */
*/

```

# Data types and literals
River comes with several builtin basic data types
The builtin types are as follows:

| Type   | C-Type       | Description                                              |
| ------ | ------------ | -------------------------------------------------------- |
| anyptr | void*        | a pointer type that can be cast down to any pointer type |
| bool   | _Bool        | boolean type                                             |
| char   | uint8_t      | alias of u8                                              |
| f32    | float        | floating point number : corresponds to C floats          |
| f64    | double       | double precision floating point number                   |
| int    | int          | signed cpu word sized integer                            |
| i8     | int8_t       | signed 8 bit integer                                     |
| i16    | int16_t      | signed 16 bit integer                                    |
| i32    | int32_t      | signed 32 bit integer                                    |
| i64    | int64_t      | signed 64 bit integer                                    |
| uint   | unsigned int | unsigned cpu word sized integer                          |
| u8     | uint8_t      | unsigned 8 bit integer                                   |
| u16    | uint16_t     | unsigned 16 bit integer                                  |
| u32    | uint32_t     | unsigned 32 bit integer                                  |
| u64    | uint64_t     | unsigned 64 bit integer                                  |
| size   | intptr_t     | signed pointer sized integer                             |
| usize  | size_t       | unsigned pointer sized integer                           |
| string | char[]       | distinct type of static array of u8s                     |
| void   | void         | used for expressions that return nothing                 |

River has the following literal types:

- null:     memory address 0x0. only applicable to pointers. compiles down to (void*)0;
- numbers:  signed/unsigned integers, floating point numbers,  
- string:   "string"
- true:     just the value true. compiles down to the expression (0==0)
- false:    just the value false. compiles down to (0!=0);

## Reserved Keyword list 

| Keyword  | Use                                        |
| -------- | ------------------------------------------ |
| true     | true                                       |
| false    | false                                      |
| fun      | function declaraction                      |
| return   | return values from functions               |
| struct   | declare a struct                           |
| enum     | declare an enum                            |
| union    | declare a union                            |
| let      | declare an immutable variable              |
| var      | declare a mutable variable                 |
| const    | declare a compile-time constant            |
| type     | declare a new type                         |
| alias    | alias a symbol to another                  |
| if       | if expression                              |
| else     | else expression                            |
| for      | for loop                                   |
| in       | get item from iterator                     |
| while    | while loop                                 |
| repeat   | repeat loop                                |
| until    | until conditional for repeat loop          |
| break    | break loop                                 |
| continue | continue to next iteration of loop         |
| defer    | execute expression at the end of the scope |
| switch   | switch expression                          |
| import   | import a module                            |
| module   | declare a module                           |
| as       | alias a module                             |

// maybe keywords

null

goto label

is

macro

impl interface


f32 f64

u8 u16 u32 u64 

i8 i16 i32 i64

usize size

uint int

string

bool

void

char

any ?

typeid

# Operators

| Operator | Use                      | Arity  | Placement |
| -------- | ------------------------ | ------ | --------- |
| +        | addition                 | binary | infix     |
| -        | subtraction              | binary | infix     |
| -        | negation                 | unary  | prefix    |
| *        | multiplication           | binary | infix     |
| /        | division                 | binary | infix     |
| %        | modulus operator         | binary | infix     |
| >        | less than                | binary | infix     |
| %        | greater than             | binary | infix     |
| &        | bitwise and              | unary  | prefix    |
| \|       | bitwise or               | unary  | prefix    |
| ^        | bitwise xor              | unary  | prefix    |
| ~        | bitwise not              | unary  | prefix    |
| .        | field / method-call      | binary | infix     |
| =        | assignment               | binary | infix     |
| +=       | add assignment           | binary | infix     |
| -=       | sub assignment           | binary | infix     |
| *=       | mult assignment          | binary | infix     |
| /=       | div assignment           | binary | infix     |
| &=       | band assignment          | binary | infix     |
| |=       | bor assignment           | binary | infix     |
| ^=       | bixor assignment         | binary | infix     |
| ==       | equality                 | binary | infix     |
| !=       | inequality               | binary | infix     |
| >=       | greater than or equal to | binary | infix     |
| <=       | less than or equal to    | binary | infix     |
| not      | boolean not              | unary  | prefix    |
| and      | boolean and              | binary | infix     |
| or       | boolean and              | binary | infix     |
| as       | type cast                | binary | prefix    |
| []       | array index              | unary  | circumfix |
| ..       | range                    | binary | infix     |


# Modules and packages
river bundles up it's code files into logical units called modules.
a river program can be made up of multiple modules, and a module can be made up of multiple `.rvr` files.

modules can share code between each other without explicitly importing the code.

a `.rvr` file must declare the module at the top of the file
```c
module foo; 
```

if a `.rvr` file does not have a module, it's filename will be taken as the module instead.
 

# variable definition & declaration

```
    var_def -> 
          ( "let" | "var" ) %id ( ":" type )? "=" expr ";"
        | ( "let" | "var" ) %id ( ',' %id )* ( ":" type )? "=" expr ( "," expr )* ";"
    ;
    
```

```ts

// variables are declared with the 'let' keyword
let x: int = 10;

// variables declared with the 'let' keyword are immutable
// to create mutable variables, use the 'var' keyword

var x: int = 10;
x += 10; // x = 20

// you can declare variables without initializing them.
// However, when just declaring a variable, you must provide it's type
let y, z, w; // illegal
let y, z, w: int; // legal

// You can assign multiple variables at once:
let y, z, w: int = 10, 30, 23;

let (x, y, z, w, e, t) = (13, 45, 35, ... = 0);
```

# Constants

you can declare compile time constants with the `const` keyword.
only primitive values and other constants can be used in the rhs of the constant declarations.
```c
const PI  = 3.1415;
const TWO_PI = PI * 2; 

const FAC3 = factorial(3); // ERROR, factorial() is not a compile time constant.
```

# pointers, arrays and slices

```rs

// pointers are defined as such:
let name: *type = &variable;
// pointers are dereferenced as such
let name: type = variable.*;

// pointers can have up to 8 levels of indirection:
let name: ********type = value; // pointer to pointer to pointer to ... 8 times to a type of value.

let ptr: ********int ; // pointer to pointer to pointer to ... 8 times to an int.

// arrays are defined as such:
let name: [size] type = value;
let arr:  [6]int = [1, 2, 3, 4, 5, 6]; // array of six integers

// arrays have their length encoded into them:
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size N will have N elements in it but the final element will have 
// the index [N-1], as indices begin at [0]
arr.len(); // => 6

// array types must always contain the size
let name: []type = value; // => ERROR, missing array size
// but for array literal definitions you can simply just put a _ to infer the length:
let arr: [_]u8 = [3, 6, 7, 2, 6]; // => Okay ✅
let arr: [_]u8; // Error: Must provide a default array

// you can initialize items in specific locations in an array using the following shorthand:
let arr: [10]bool = [
    3 = false,
    6 = true,
];

// you can initialize an array or a portion of an array with a value using the ... operator
let buf: [512]u8 = [... = 0x42];
let foo: [10]u8 = [1, 4, 6, ... = 32];

// similarly, you can also assign elements over ranges:
let mem: [512]u8 = [
    0x00..0xFF   = 0x42,
    SPECIAL_BYTE = 223,
    OTHER_BYTE   = 243,
    FLAG_BYTE    = Flag.Foo | Flag.Bar;
    0x120..0x200 = 0x00,
]

// for multi-dimentional arrays
let map: [10][10]f32 = [...0];

let map: [M_WIDTH][M_HEIGHT][M_LENGTH]int;

// arrays can be pointed to too:

let arr: *[6] int ;  // pointer to an array of 6 ints
let arr:  [6]*int ;  // array of 6 pointers to ints
let arr: *[6]*int ;  // pointer to array of 6 pointers to ints

// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
fun print_num_array ( numbers: [10]int ) = { /*...*/ }

let nums: *int;
print_num_array(nums); // ERROR: Incompatible types; expected type int[10], got ^int instead;

// slices are views into contiguous elements in memory, kinda like an array, but they point to memory rather than contain it directly
// basically, slices are the equivalent of doing something like this in C:
//
// struct {
//     int n_elems;
//     elem* elems;
// };
//
// n_elems = 512;
// elems = malloc(sizeof(elem) * n_elems);
// except instead of doing all that, it's just a self-contained type with a len and a pointer part.
// the length of slices are known at run-time, rather than at compile time like with arrays

let arr: [_]u8 = [4, 6, 7, 8, 3, 7];
let slice: [*]u8 = arr[3..5]; // => [8, 3, 7], len 3

let mem: [*]u8 = mem.alloc(u8, 100);
mem.len(); // => 100

// like with regular pointers, pointers to arrays can have up to 8 levels of indirection:
// NOTE: Maybe this is like, stupid as heck???
let arr: ********[6]int; // pointer to pointer to pointer to ... 8 times to an array of 6 ints;
let arr: ********[6]********int; // pointer to pointer t ... 8 times an array of 6 pointers to pointers to ... 8 times to an int;

```

# strings
```rs
// the string type in river is an alias of [_]char
let s: string = "hello world";
s.len(); // 11

let sp = s.split(6)[0]; // "hello"
    
```

# Control flow

```c

if (expression) {
    // do stuff
} else if (other_expression) {
    // do other stuff
} else {
    // do yet more stuff
}
// if statements in river are actually expressions, and can return values
let x = if (expression) {
    value
} else (expression) {
    value
};

// when if statements are being used as an expression, i.e they're expected to produce a value, you MUST provide an else clause in case the if check fails:
let x = if (false) 10; // ERROR: x never gets initiated :(
let x = if (false) 10 else 20; // OK

let x = if (bool) 10 else if (other_bool) 11 else 23;

// switch statements are also actually expressions, and can pattern match
switch x {
    case => ...,
    case => {...},
    // cases don't have implicit fallthrough and a fallthrough must be explicitly forced via use of the `fallthrough` keyword
    case => {
        ...
        fallthrough
    },
    // cases can be chained together
    case | case | case => {...},
    // default catch case
    default => {...} 
}

// when switch cases are being used as expressions, they too must either exhaustively match all cases or provide a default fallback.

fun x_to_string(x: X): string =
    switch x {
        X_XX => "XX",
        X_XXX => "XXX",
        X_XXXX => "XXXX"
        default => "bleh"
    };

// all the loops are statements, and not expressions.

// while loops are pretty normal
while (expression) { ... };
while (expression) expression;

// repeat..until statements repeat an expression until the condition is held true
// they're kinda like an inverse do while, where do..while repeats until a condition is false, repeat..until repeats until a condition is true
repeat {

} until expression;

let x = 0;
repeat x++ until x == 10;

// river has the classic C style for loops available to use
for(init;cond;inc) {...}
for(init;cond;inc) expression;

// river also has a for-each style loop that iterates over a collection:
for (let item in list) {

}

// the for-in loop uses pattern matching to destructure collections
for (let index, value in list) {
    do_something();
}

```

# Structs 

```c

// You can wrap up several datatypes into a big datatype called a struct, just like in C 
type Vector3 = struct {
    x, y, z: f32;
};

type User = struct {
    // Struct fields can have default values
    username: string = "Admin";
    pin: u8;
};

let admin: User;
printf("user.name = %s", admin.username); // => "user.name = Admin"
admin.name = "Administrator";
printf("user.name = %s, user.pin = %i", admin.username, admin.pin) // => "user.name = Administrator, user.pin = 0000"; 

type Vec4 = struct(x, y, z, w: float = 1) {
    x, y, z, w: float;
}

// struct field tags
// you may tag struct fields with a string which attaches meta info to the struct
type GameObject = struct {
    pos: Vec3 `json:"pos"`;
    transform: Mat4 `json:"transf"`;
    active: bool // untagged
};

```

# Tuples
```c 

// tuples are like structs, except they don't have named parameters
let tup: (int, string) = (23, "River");
tup.0; // 23
tup.1; // "River"

// you can destructure a tuple into multiple variables.
let (age, name) = tup;

fun get_person(id: int): (int, string)  {
    return (people[id].0, people[id].1);
}

let person = get_person(id);

type Colour = (u8, u8, u8, u8);
let red: Colour = (255, 0, 0, 255);
```


# Functions 

```
    function_def -> "fun" %ID ( "[" generic_list "]" )? ( "(" params_list ")" )? ( "->" %ID )? "=" expr ";" .   
```

```c

// NOTES:
//
// * Function return types MUST be explicit
// * Functions MUST return values explicitly, unless using = syntax

fun sub ( x, y: int ): int {
    return x - y; // returns x - y
}

// for one-liner functions you can just use an expression or a statement in the body.
// the return type is inferred:
fun sub ( x, y: int ) = x - y;

// function types can be declared as such:
// this is a function that has a parameter of type int and returns a value of type bool
type func = fun(int): bool;

fun smth(i: int, callback: func): bool = not callback(i);

fun do_something(): (foo, Maybe[bar]) = {
    
}

// Functions support default parameter values
// If an argument is not passed for a particular parameter,
// the default value is used instead
// Note: default values must be at the end of the function signature
fun Colour_from_rgba( r, g, b: int, a: int = 255 ): Colour =
    Colour {
        r, g, b, a
    };

// Arguments with default parameters can be omitted
Colour_from_rgba(125, 125, 125); // param 'a' => 255;
Colour_from_rgba(125, 125, 125, 125); // param 'a' => 125;

// Function arguments can be named when calling functions
// Take the following function for example:
fun draw_rect ( x, y, width, height: int, rotation: int, col: Colour ): Rectangle {
    /*...*/
}

// You can call the function by naming the individual arguments
let rect = draw_rect (
    x = 20,
    y = 30,
    width = 240,
    height = 360,
    col = (10,10,10,255)
);
// The advantage of named arguments is that you can rearrange the order in which you pass them:
draw_rect_pro(
    width = 240,
    height = 360,
    x = 20,
    y = 30,
    rotation = 10,
    col = Color { 10,10,10,255 }
    ); // perfectly valid

// NOTE: when mixing and matching named vs unnamed arguements, the unnamed arguements must always come first and in order:
draw_rect_pro(
    20, // x
    30, // y
    width = 240,
    height = 360,
    rotation = 10,
    col = Color { 10,10,10,255 }
    ); // perfectly valid
draw_rect_pro(
    width = 240,
    height = 360,
    20, // x
    30, // y
    rotation = 10,
    col = Color { 10,10,10,255 }
    ); // ERROR

// functions can have multiple return values by utilizing tuples
fun read_file(path: string): (File, string) {
    // ... file reading logic ...

    return file, extension;
}


let file, ext = read_file("SPEC.md");
if ext == ".md" {
    parse_markdown(file);
}

// In the case of functions that return nullable values, you can unwrap
// the value using the '?' operator:
let name = get_name()?; // => if get_name() returns null then the program panics;

// for one liners without params the parenthesis can be omitted 
fun consume() => scanner.char += 1; // note, how do you decide whether or not this function returns a value, without a hint? the rhs of this function is an assignment expression, what does that evaluate to?
fun peek = scanner.token[scanner.char + 1];

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
fun example_func( val: float , func: fun(float, int) ): u32 = {
    func(val, 20); // => calls the passed-in function
}

// functions can return other functions
fun func(): fun(bool): int = {
    // functions can be defined inline as closures
    return fun(bool x): int {
        if x {
            true
        } else {
            false
        }
    };
}

// though it's nicer to just give the return functions a type alias
type callback = fun(int, float): bool;

// methods

type Vec2 = struct {
    x, y: int;
};

fun Vec2:add(this, rhs: Vec2) -> Vec2 = { ... };
fun Vec2:neg(self) = { ... };

let x = Vec2(10, 10);
let y = x.neg();  // mutates x
let z = x:add(y): // does not mutate x
let w = Vec2:add(z, y);

let x = ({
    // do stuff here 
});


```

# Enums 
```c 

// similar to C's enums, except with their own namespace

type Colour = enum {
    Red, Blue, Green, White, Yellow, Brown
};

type MnMs = enum {
    Red, Blue, Green, Yellow, Brown, Orange
};

let x = Red; // => Error: Assigning variable <x> to nknown value <Red>;
let x = Colour:Red; // => compiles
let y = MnMs:Red; // => Also compiles

// enums can act as discriminated unions:
type Literal = enum {
    Int: long,
    Float: double,
    String: std::String,
    char: char
}



```

```unions

type Vec3 = union {
    raw: f32[3];
    _: struct {
        x, y, z: f32;  
    };
}

    
let v: Vec3 = Vec3(10, 20, 30);
v.x += 10;
print(v); // => {20, 20, 30}
```



# Custom Types

```c

// You can define your own types by aliasing other types
alias Integer = int;
let x: Integer = 10; // base type: int

// aliased types are interchangable with their base types without the need for an explicit cast

let y: int = x; // valid

// You can also define distinct types with the use of the 'type' keyword:
type Celcius = double;

let temp: Celcius = 35.0;
let f: double = temp; // illegal, cast via `as double`
let f: double = temp as double; // ok 👍

// Types can be composed of multiple base types
// this type can represent both u32 or f32 values
type number -> int | float;

number x = 10; // valid
number y = 52.32; // also valid
number z = 42.4d; // not valid, as it is a double value

```

## File imports 

```c

// in the C backend, `import`ed variables and functions just get pasted in as forward
// declarations.
// then later, the linker compiles the definition in with the parent module's .o file

// import modules with the import keyword
import std:io;

fun 
main() {
    io:println("Hello!");
}

import std:math;

m:abs();


import std:io;
using namespace io;

fun
main() {
    
}



```

# Statements and Expressions

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

val y = if x then x;

val v = if x then if y then if z then if w then a else 0;
// is equal to
val v = if x && y && z && w then a else 0;


// Statements

val x = 10; // statement
funcCall();
```

# Generics
```rs
// generic structs

type Buf<T> = struct {
    idx: size;
    data: [*]T;

    fun Buf<T>();
};

let intarr = Buf<int>();
fun sub[T: Subtractive](x, y: T): T = x - y;

    
```


# Misc + Reference

## Character / operator list:
// arithmetic operators

    + - * / %

// equality and comparisons

    != == <= >= > <

// logical operators 

    not and or

// bitwise operators

    & | ~ ^

// assignment 
+= -= *= /= %=

// single comment

/*
    multiline comment  
*/

/// doc-comment

//*
    multi-line doc-comment
*//


 ^ ~ & && ||
. , ? : ; ' " = ( ) { } [ ]
@ .. ... -> =>

# Grammar specification

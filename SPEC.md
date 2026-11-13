# Introduction
This is the language spec for river.
for now this is more of a sketchpad for features, but it'll get leaner and cleaner with each commit!

trailing commas in lists allowed

considerations:
    null safety
    error handling
    interfaces

## Reserved Keyword list 

| Keyword  | Use                                        |
| -------- | ------------------------------------------ |
| alias    | alias a symbol to another                  |
| as       | alias an import module, type cast          |
| break    | break loop                                 |
| case     | case branch for switch expression          |
| const    | declare a compile-time constant            |
| continue | continue to next iteration of loop         |
| defer    | execute expression at the end of the scope |
| do       | do branch in inline for/while loops        |
| else     | else expression                            |
| enum     | declare an enum                            |
| false    | false                                      |
| for      | for loop                                   |
| fun      | function declaration                       |
| goto     | goto statement                             |
| if       | if expression                              |
| import   | import a module                            |
| in       | get item from iterator                     |
| let      | declare an immutable variable              |
| raw      | mark a union as an untagged union          |
| return   | return values from functions               |
| struct   | declare a struct                           |
| switch   | switch expression                          |
| then     | then branch in if expression               |
| true     | true                                       |
| type     | declare a new type                         |
| undef    | leave a variable undefined                 |
| union    | declare a union                            |
| var      | declare a mutable variable                 |
| while    | while loop                                 |

// maybe keywords

null

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
| *        | multiplication           | binary | infix     |
| /        | division                 | binary | infix     |
| %        | truncated modulo         | binary | infix     |
| %%       | floored remainder        | binary | infix     |
| -        | negation                 | unary  | prefix    |
| *        | pointer deference        | unary  | prefix    |
| <        | less than                | binary | infix     |
| >        | greater than             | binary | infix     |
| &        | bitwise and              | unary  | prefix    |
| \|       | bitwise or               | unary  | prefix    |
| ^        | bitwise xor              | unary  | prefix    |
| ~        | bitwise not              | unary  | prefix    |
| :=       | assignment               | binary | infix     |
| ++       | increment                | unary  | postfix   |
| --       | decrement                | unary  | postfix   |
| +=       | add assignment           | binary | infix     |
| -=       | sub assignment           | binary | infix     |
| *=       | mult assignment          | binary | infix     |
| /=       | div assignment           | binary | infix     |
| &=       | band assignment          | binary | infix     |
| |=       | bor assignment           | binary | infix     |
| ^=       | bixor assignment         | binary | infix     |
| ==       | equality                 | binary | infix     |
| !=       | inequality               | binary | infix     |
| <=       | less than or equal to    | binary | infix     |
| >=       | greater than or equal to | binary | infix     |
| !        | boolean not              | unary  | prefix    |
| &&       | boolean and              | binary | infix     |
| \|\|     | boolean and              | binary | infix     |
| as       | type cast                | binary | prefix    |
| .        | field / method-call      | binary | infix     |
| []       | array index              | unary  | circumfix |
| ..       | range                    | binary | infix     |
| ->       | function applicator      | binary | infix     |


# Data types and literals
River comes with several builtin basic data types
The builtin types are as follows:

| Type   | C-Type       | Description                                              |
| ------ | ------------ | -------------------------------------------------------- |
| any    | void         | meant to only be used as a pointer, any*                 |
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

# Modules and packages
river bundles up it's code files into logical units called modules.
a river program can be made up of multiple modules, and a module can be made up of multiple `.rvr` files.

files in the same module can share code between each other without explicitly importing the code.
that is to say, if a file `fooA.rvr` and a file `fooB.rvr` share the same module, then they also share the same namespace, and do not have to `import` code from each other.

modules come in two forms: directory modules and file modules.
by default, every file inside a directory is considered to be a single self-contained module.

however, if a directory has a `mod.rvr` file, then that directory and every file within the directory is considered to be the same module.
a directory may not have more than one module or `mod.rvr` file at a time in this way.

a directory that has a `mod.rvr` file in it ( which will be referred to as a directory module from here on out ) may itself contain subdirectories.
these subdirectories are treated as submodules if they themselves are directory modules.
otherwise, the files within them are treated as file modules and are taken to be in the same module level.
i.e if a module `foo` has a subdirectory `bar` with three file modules `a`, `b`, and `c`, then those file modules are accessed like `foo.a/b/c` instead of `foo.bar.a/b/c`.

here's an example module hierarchy:
```
src/
|- main.rvr
|- input.rvr
|- window.rvr
|- graphics/
   |- mod.rvr
   |- mesh.rvr
   |- model.rvr
   |- texture.rvr
|- physics/
   |- 2d/
      |- mod.rvr
      |- shapes.rvr
      |- collisions.rvr
   |- 3d/
      |- mod.rvr
      |- shapes.rvr
      |- collisions.rvr
   |- debug.rvr
|- serialize/
   |- json.rvr
   |- binary.rvr
   |- otherFormat.rvr
```

in the given hierarchy, there's 6 top-level modules: `main`, `input`, `window`, `graphics`, `physics`, and `serialize`.
`graphics` is a single module with no submodules.
`physics` contains three submodules, two in the form of module directories: `2d`, `3d`, and `debug`.
`serialize` contains three submodules, in the form of file modules: `json`, `binary`, and `otherFormat`.

module imports must never be cyclical i.e if module `a` imports module `b`, and module `b` imports module `a`, then that's an error.
the common code from modules `a` and `b` should be moved to a new module `c`.

the `main` module is treated as the entrypoint for the final executable and must be present.
the `main` module can be renamed and does not have to reside in `main.rvr`, as long as there *is* a `main` module present in the project.

for libraries, a `lib` module is used as the entrypoint instead.


# Compiler directives

- turn off reflection ( per datatype / per file / per module / per project )
    ```c
        @no-reflect
        struct Foo {
            x: Bar
            y: Baz
        }
        // ---
        @no-reflect:file

    ```
- turn off automatic struct packing/alignment ( per struct )
    


# Identifiers and Numbers

## Identifiers
```
iden -> [a-zA-Z_] [a-zA-Z0-9_-]* 

identifier
_identifier
some_identifier
other-identifier // yes, hyphenated identifiers are allowed    

// valid identifiers
_ // valid identifier, but has special semantic value
a
_a
_3
abc
_abc
_a_bc
_0_10
a-b
a-b_c
_0-1_2
__
_-
_---
_-_
_0__0_

```

## Numbers

```
digit ->
    [0-9]

number ->
    | float
    | integer


dec_exp -> [eE] [-+]? integer ;

bin_exp -> [pP] [-+]? integer ;


float -> 
    | digit* ( '.' digit+ )? dec_exp? [fFdD]?
    | hex_literal* ( '.' hex_literal+ )? bin_exp? [fFdD]? 


integer ->   
    (
        digit* ( ['_] digit+ ) dec_exp?
        | hex_literal bin_exp?
    		| octal_literal
    		| binary_literal
    )
    ( [uUiI] ( \d* | 'z' ) )?


binary_literal ->
    '0b' binary_digit* ( [_'] binary_digit+ )?
hex_literal ->
    '0x' hex_digit* ( [_'] hex_digit+ )?
octal_literal ->
    '0c' octal_digit* ( [_'] octal_digit+ )?


hex_digit ->
    [0-9a-fA-F]
octal_digit ->
    [0-7]
binary_digit ->
    [01]

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


## File imports 

```c

// in the C backend, `import`ed variables and functions just get pasted in as forward
// declarations.
// then later, the linker compiles the definition in with the parent module's .o file

// import modules with the import keyword
import std::io;

fun main() {
    io::println("Hello!");
}

// import name aliasing
import std::math as m;

m::abs();


import std::io;
using io; // expands io into current scope.

fun main() {
    println("Hello");
}

// import multiple submodules
import std::{using io, math as m};

fun main() {
    println("val: %v", m::sqrt(9)); // "val: 3"
}

```

# Constants
```
    const_def -> "const" iden ":" type ":=" expr ";" ;
```

you can declare compile time constants with the `const` keyword.
constants are akin to `constexpr` in C++, they alias a symbol to an expression.
```rust

const PI: f32 := 3.1415;
const TWO_PI: f32 := PI * 2; 

```

# Variable declaration & definition

```
    var_decl ->
          | ("let" | "var") iden ":" data_type ";"
          | ("let" | "var") iden ":" data_type? ":=" LITERAL ";"
          | ("let" | "var") iden ( "," iden )+ ":" data_type ";"
          | ("let" | "var") iden ( "," iden )+ ":" data_type  ":=" LITERAL ( "," LITERAL )+ ";"
          ;
```

```ts

// variables are declared with the 'let' keyword
let x: int := 10;

// variables declared with the 'let' keyword are immutable
// to create mutable variables, use the 'var' keyword

var x: int := 10;
x += 10; // x = 20

// variable types can also be inferenced
let foo := 10; // => int
let bar := 3.1415; // => float

// you can declare variables without initializing them.
// However, when just declaring a variable, you must provide it's type
let w; // illegal
let w: int; // legal

// all variables are initialized to a zero value on declaration
// each datatype in river has it's own associated default or 'zero' value
let x: int; // x = 0

// if a variable is supposed to be uninitialized, use the `undef` keyword:
let foo: int := undef;

// you may declare multiple variables at once, provided they're of the same type:
let r, g, b, a: u8;
// you may also assign values to said variables in the order of definition.
// NOTE: this is only limited to variable declaration, and does not work in other contexts
// ( i.e struct / function default parameters )
let r, g, b, a: u8 := 0xFF, 0x05, 0x24, 0xFF;
```

# Pointers

```rs

// pointers are defined as such:
let name: *type := &non_pointer_variable;
// pointers are dereferenced as such
let name: type := *pointer_variable;
```
pointer arithmetic is disallowed.
the only operations a pointer has are the address-of and deference operations.
See [`Slices`](#Slices) for pointer-arithmetic.
Null pointers also do not exist. instead, nullable pointers are wrapped up in an [`Optional`](#Optionals) type.

# Arrays

```rs
// arrays are defined as such:
let name: [size]type := value;
let arr:  [6]int := {1, 2, 3, 4, 5, 6}; // array of six integers

// arrays have their length encoded into them:
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size N will have N elements in it but the final element will have 
// the index [N-1], as indices begin at [0]
arr.len(); // => 6

// array types must always contain the size
let name: []type := value; // => ERROR, missing array size
// but for array literal definitions you can simply just put a _ to infer the length:
let arr: [_]u8 := {3, 6, 7, 2, 6}; // => Okay ✅
let arr: [_]u8; // Error: Must provide a default array

let arr := [5]u8.{0, 1, 2, 3, 4}

// you can initialize items in specific locations in an array using the following shorthand:
let arr: [10]bool := {
    3 = false,
    6 = true,
};

// you can initialize an array or a portion of an array with a value using the ... operator
let buf: [512]u8 := {... := 0x42};
let foo: [10]u8 := {1, 4, 6, ... := 32};

// similarly, you can also assign elements over ranges:
let mem: [512]u8 := {
    0x00..0xFF   := 0x42,
    SPECIAL_BYTE := 223,
    OTHER_BYTE   := 243,
    FLAG_BYTE    := Flag.Foo | Flag.Bar;
    0x120..0x200 := 0x00,
};

// for multi-dimentional arrays
let map: [10][10]f32 := {...0};

let map: [M_WIDTH][M_HEIGHT][M_LENGTH]int;

// arrays can be pointed to too:

let arr: *[6] int ;  // pointer to an array of 6 ints
let arr:  [6]*int ;  // array of 6 pointers to ints
let arr: *[6]*int ;  // pointer to array of 6 pointers to ints

// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
fun print_num_array ( numbers: [10]int ) { /*...*/ }

let nums: *int;
print_num_array(nums); // ERROR: Incompatible types; expected type int[10], got ^int instead;

```

c output:
```c

// let arr:  [6]int =
int arr_data[6] = {1, 2, 3, 4, 5, 6};
rvrArray arr = (rvrArray){
    .len = 6,
    .elem_size = sizeof(int)
    .data = (void*)arr_data
};
    
```

# Slices

slices are views into contiguous elements in memory, similar to an array, but they point to memory rather than contain it directly.
they are equivalent to what are commonly referred to as fat pointers in C, that is, they are a pointer to some memory paired with the size of that memory.
basically, slices are the equivalent of doing something like this in C:

```c
struct Slice {
    size_t n_elems;
    elem_t *_elems;
};

Slice s;
s.n_elems = 512;
s._elems  = malloc(sizeof(elem_t) * s.n_elems);
```

the length of slices are known at run-time, rather than at compile time like with arrays.

```rs


let arr: [_]u8 := [4, 6, 7, 8, 3, 7];
let slice: [*]u8 := arr[3..5]; // => [8, 3, 7], len 3

let mem: [*]u8 := mem.alloc(u8, 100);
mem.len(); // => 100

// like with regular pointers, pointers to arrays can have up to 8 levels of indirection:
// NOTE: Maybe this is like, stupid as heck???
let arr: ********[6]int; // pointer to pointer to pointer to ... 8 times to an array of 6 ints;
let arr: ********[6]********int; // pointer to pointer t ... 8 times an array of 6 pointers to pointers to ... 8 times to an int;

```

# Strings

```rs
// the string type in river is an alias of [_]char
// so it implicitly stores a length variable
let s: string := "hello world";
s.length; // 11

let sp := s.split(6)[0]; // "hello"
```

river supports three types of string literals:
- normal literals
- multiline literals

```rs

// normal string literals can have character escape sequences
// such as \n, \t etc
let str := "hello there \n how are you?"

io::println(str); // hello there
                  // how are you?

// multiline strings span multiple lines
let str :=
    \\ hi hello
    \\ this is a multiline string yaaay
    \\
    \\     this is an indented line
    ;

```

however, if that's unsavoury for you for whatever reason, then river still supports C's style of automatically concatenating string literals next to each other.
though of course, in this case you'd have to provide formatting and newlines yourself.

```c
let str =
    "hello\n"
    "this is a multi-line string\n"
    "\n"
    "\tthis is an indentation woo\n";    
```

# Control flow expressions

## If Expression
```rs

// if-statements in river are actually expressions, and can return values
let x := if condition {
    value
} else condition {
    value
};

// non expression format. implicitly returns void
if condition {
    // do stuff
} else if other_condition {
    // do other stuff
} else {
    // do yet more stuff
}


// if-expressions don't need parenthesis for their condition unlike in C.
// instead, the body MUST be contained in a block
if bool { something() } else { something_else() }

// when if-statements are being used as an expression, i.e they're expected to produce a value, you MUST provide an else clause in case the `if` check fails:
let x := if false { 10 }; // ERROR: x never gets initiated :(
let x := if false { 10 } else { 20 }; // OK

```

## Else Expressions
```rs

// else expressions can be used on their own to provide fallback values in case of failure:
let config := read_config() else default_config;
// here the else expression only gets triggered if read_config() results a falsey value.

// similarly

```

### If-Let/Var Expression

```rs

// if let or if var expressions is this thing i'll come up with a description later
if let out := func() {
    do_something_with_out(out);
} else {
    do_something_else_with_out(out);
}

do_something_with_out(out); // ERROR: Out not declared.


// can run additional checks ( up to one )
if let out := func(); out != null {
    do_something_with_out(out);
} else {
    do_something_else_with_out(out);
}


```

## Switch expression
    
```rs

const foo := 10;
let num := getSomeNum();

let val := switch num {
    0 => "string",

    // you can match multiple cases at once with the | operator
    1 | 2 | 3 => "other string",

    // you can also match over ranges, provided the type supports the range operation
    4..6 => "yet another string",

    // you can also match over a compile time constant
    foo => "blah blah blah",

    // cases don't have implicit fallthrough and a fallthrough must be explicitly forced via use of the `nextcase` keyword
    52 => nextcase,
    53 => "yaaay",

    default => {...}
}

// when switch cases are being used as expressions, they must either exhaustively match all cases or provide a default fallback.
fun x_to_string(x: X): string =>
    switch x {
        X_XX    => "XX",
        X_XXX   => "XXX",
        X_XXXX  => "XXXX",
        default => "bleh"
    };

// river also supports another switch construct called an anonymous switch:
let foo, bar, baz: bool;

let v := switch _ {
    foo => 10,
    bar => 20,
    baz => 30,
    else => 40,
};

    
```

# Loop statements

## While loop
all the loop constructs are statements, and not expressions.
    
```c

// while loops are pretty normal
while expression { ... }

```

## Do-While loop
    
```c

// do..while loops repeat an expression while the condition is held true
do {

} while expression;

let x := 0;
do { x++ } while x < 10;
```

## For loops
    
```rs

// river has the classic C style for loops available to use
for init;cond;inc {...}

// NOTE: the `let` here means that it's illegal for it to be mutated *within* the loops body
for let x := 0; x < arr.len; x++ {
    io.println("x is: {}", arr[x]);
}

// the previous snippet is better written using a for..in loop:
for let i, x in arr {
    io::println("x[{}] is: {}", i, x);
}

for let i, x in [0..10] {
    io::println("{}", x*x)
};


var arr: [20]int = {...};
// the for-in loop uses pattern matching to destructure collections
for let (index, value) in arr.enumerate() {
    arr[index] = value;
}

```

# Custom types and symbol aliasing

```c

// You can define distinct types with the use of the 'type' keyword:
type Celcius := double;

// types defined by the `type` keyword are distinct from their base types:
let temp: Celcius := 35.0;
let f: double := temp; // illegal, cast via `as double`
let f: double := temp as double; // ok 👍
```

# Structs 

```rs

// You can wrap up several datatypes into a big datatype called a struct, just like in C 
struct Vector3 {
    x, y, z: f32;
}

// struct fields can have default values
struct Foo {

    // ❌ illegal
    r, g, b: u8 := 0, 1, 2;

    // ✅ allowed
    r: u8 := 0;
    g: u8 := 1;
    b: u8 := 2;
}


let v: Vector3;
v.x := 1;
v.y := 3;
io::println("{%v}", v); // => Vector3 { x: 1, y: 3, z: 0 }

struct Entity {
    id: string;
    pos: Vector3;
    health: int;
}

// struct initialization
let e: Entity := .{"newEntity", Vector3.{10, 10, 10}, 10};

// alternatively, the struct fields can be named on initialization
let e: Entity := .{   
    .id := "newEntity",
    .pos := Vector3.{10, 10, 10},
    .health := 10
};

// if the type isn't provided during declaration, 
let e := Entity.{"newEntity", .{10, 10, 10}, 10};


// struct fields may have default values, that are automatically inserted on initialization
struct Person {
    name: string := "John Doe";
    admin: bool := false;
    id: int; // no custom default value
}

// default struct initialization
let x := Entity.{}; // => Entity.{ "John Doe", false, 0 };


// struct fields may also set their default values relative to other fields:
struct Rectangle {
    height: int := 1;
    width:  int := 1;

    area: int := height * width;
    perim: int := 2 * height + 2 * width; 
}

let x := Rectangle.{}; // => Rectangle { .height = 1, .width = 1, .area = 1, .perim = 4 }
let y := Rectangle.{10, 10}; // => Rectangle { .height = 10, .width = 10, .area = 10, .perim = 40 }

// TODO: Needs more work
// structs may subtype one other struct
struct Tile : Rectangle {
    tileId: int;
}

// TODO: Needs more work
// struct field tags
// you may tag struct fields with a string which attaches meta info to the struct
struct GameObject {
    pos: Vec3 `json:"pos"`;
    transform: Mat4 `json:"transf"`;
    active: bool; // untagged
}


// struct reflective fields:
// - size: returns the static size of the struct
// - members: returns a static array containing struct member information

// TODO needs more work
// anonymous struct literals and types are treated as structural and can be coerced into named structs
fun foo(): struct{ x, y, z: f32 };

let x: Vector3 := foo(); // Okay

fun bar(v: struct{a, b, c, d: u8});

let c := Colour.{0xff, 0x00, 0x00, 0x00};
bar(c); // also okay
let d := .{0, 0, 1, 255};
bar(d); // also okay

// anonymous structs may not have default values


```

# Tuples
```c 

// tuples are like structs, except they don't have named parameters
let tup: (int, string) := (23, "River");
tup.0; // 23
tup.1; // "River"

// you can destructure a tuple into multiple variables.
let age, name := tup;

fun get_person(id: int): (int, string)  {
    return people[id].0, people[id].1;
}

let person := get_person(id);

type Colour := (u8, u8, u8, u8);
let red: Colour := (255, 0, 0, 255);

// NOTE: tuple types MUST be wrapped in parenthesis
//     > however, tuple literals can omit the parents
//     > and just use commas, like in python.
//     > unless passing into a function parameter
```

# Enums 
```rs

// enums are similar to C's enums, except with their own namespace
enum Colour {
    Red,
    Blue,
    Green,
    White,
    Yellow,
    Brown,
}

let x := Red; // => Error: Assigning variable <x> to unknown value <Red>;
let x := Colour::Red; // => compiles
let y: Colour := Red; // => Also compiles


// you can specify the backing type of an enum ( integer only )
enum Something : u8 {
    Something,
    OtherThing
}

// you can assign values to your enumarations
// enums are ordered, so if you leave unassigned gaps they just take on the values of the last assigned value + N where N is the distance from the last assigned value
// this only applies for integer based enums of-course.
enum Flags {
    Clear = 1,
    Stop  = 1<<1,
    Start = 1<<2,
    Idk   = 512,
    Other, // 513
}

// enums with explicit backing types can be used in place of the backing type, or any type that they can be implicitly cast to:
fun foo(num: u8) {...}
fun bar(num: u16) {...}

foo(Something::OtherThing); // okay
bar(Something::OtherThing); // also okay


// otherwise you'd have to manually cast:
fun baz(num: f32) {...}

baz(Something::OtherThing); // error
baz(Something::OtherThing as f32); // OK 👍


// enums have various reflective fields:
// - first: get the first element of an enum
let x := TokenTag::KeywordTag.first; // => TokenTag[.KeywordTag].Return
// - last: get the last element of an enum
let x := TokenTag::KeywordTag.last; // TokenTag[.KeywordTag].Else
// - len: get the total number of elements in an enum
for c in Colour.len { something(c); }
// - values: get an array with all enum values
// - names: get an array with the names for all enums

```

# Unions

```rs

// unions are pretty standard, only one item can be active at a time, the size of the union is the same as that of it's largest member
// unions in river are discriminated by default
union Literal {
    None,
    Int: int,
    Float: f64,
    String: std::String,
    Char: char
}

let intLit = Literal::Int(19);

let v = someLiteralFunction();

switch v {
    Int(v)    => printf("Int: {}", v);
    Float(v)  => printf("Float: {}", v);
    String(v) => printf("String: {}", v);
    Char(v)   => printf("Char: {}", v);
}

union Maybe[T] {
    None,
    Some: T,
}

union Result[T, E] {
    Error: E,
    Ok: T,
}

fun readFile(path: string): Result(File, FileError) {
    let f = file.open(path);
    switch f {
        Error[e] => {
            
        },
        Ok[f] => {}
    }
}

// unions can be untagged too, if desired:
union Thingy {
    Int: int,
    Float: float,
}

```

# Functions 

```
    function_def ->
        "fun" %id ( "(" params_list ")" )? ( ":" type )? ( ("=>" blockless_expr ";") | block_expr )
        ;   
```

```c

// NOTES:
//
// * Function return types MUST be explicit, unless using => syntax

fun sub ( x: int, y: int ): int {
    return x - y; // returns x - y
}

// functions implicitly return the last expression in the body:
fun make_point ( x, y: int ): Point {
    Point.{x, y} // implicitly returned
}

// for one-liner functions you can just use an expression or a statement in the body.
// the return type is inferred:
fun sub ( x, y: int ) => x - y;

// NOTE: function results MUST be used.
sub(10, 20); // ❌ ERROR: unused function value;

x = sub(10, 20); // ✅ Okay :) 

// if you do not want to use a function return value, simply assign the value to _

_ = sub(10, 20); // ✅ Also okay

// function types can be declared as such:
// this is a function that has a parameter of type int and returns a value of type bool
type func = fun(int): bool;

fun smth(i: int, callback: func): bool => not callback(i);

// Functions support default parameter values
// If an argument is not passed for a particular parameter,
// the default value is used instead
// Note: default values must be at the end of the function signature
fun Colour_from_rgba( r, g, b: int, a: int = 255 )
    => Colour {
        r, g, b, a
    };

// Arguments with default parameters can be omitted
Colour_from_rgba(125, 125, 125); // param 'a' => 255;
Colour_from_rgba(125, 125, 125, 125); // param 'a' => 125;

// default function arguments also support type inferencing, but only for
// values known at compile time ( such as constants, and literals )
fun new_object(pos: vec3, alloc := default_allocator): Object {}

// here, `default_allocator` is presumed to be some global constant, and it's
// type is inferred ( to be Allocator, the type of the global constant )

// Function arguments can be named when calling functions
// Take the following function for example:
fun draw_rect ( x, y, width, height, rotation: int, col: Colour ): Rectangle {
    /*...*/
}

// You can call the function by naming the individual arguments
// this uses the same syntax as named struct fields
let rect = draw_rect(
    .x = 20,
    .y = 30,
    .width = 240,
    .height = 360,
    .col = Color.{10,10,10,255}
);
// The advantage of named arguments is that you can rearrange the order in which
// you pass them, irrespective of the original ordering:
draw_rect_pro(
    .width = 240,
    .height = 360,
    .x = 20,
    .y = 30,
    .rotation = 10,
    .col = Color.{ 10,10,10,255 }
    ); // perfectly valid

// NOTE: when mixing and matching named vs unnamed arguements, the unnamed arguements must always come first and in order:
draw_rect_pro(
    20, // x
    30, // y
    .width = 240,
    .height = 360,
    .rotation = 10,
    .col = Color.{ 10,10,10,255 }
    ); // ✅ perfectly valid

draw_rect_pro(
    width = 240,
    height = 360,
    20, // x
    30, // y
    rotation = 10,
    col = Color { 10,10,10,255 }
    ); // ❌ ERROR 

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

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
fun example_func( val: float , func: fun(float, int) ): u32 => {
    func(val, 20); // => calls the passed-in function
}

// though it's nicer to just give the return functions a type alias
type callback = fun(int, float): bool;

```

# Impl blocks

every type has it's own namespace in river. impl blocks define what goes in these namespaces:

```rs

// normal vector3 struct
struct Vector3 {
    pub x, y, z: f32;
}

// impl block for the vector namespace
impl Vector3 {
    // constructor functions
    pub new ( v: f32 ) => Self.{v, v, v};
    pub new ( x, y: f32 ) => Self.{x, y, 0};
    pub new ( v: Vector2 ) => Self.{v.x, v.y, 0};

    // constants
    pub const Up: Self    = Self( 0,  1, 0 ),
    pub const Down: Self  = Self( 0, -1, 0 ),

    pub const Right: Self = Self(  1, 0, 0 ),
    pub const Left: Self  = Self( -1, 0, 0 ),

    pub const Front: Self = Self( 0, 0,  1 ),
    pub const Back: Self  = Self( 0, 0, -1 ),
}

let up  = Vector3::Up; // Vector3(0,1,0);
let neg = Vector3(-1); // Vector3(-1,-1,-1);


// you may also set a file to be the impl for a type.

// types.rvr
struct Vec2 { ... }
struct Vec3 { ... }
struct Mat4 { ... }

// Vec2.rvr
impl Vec2;

fun add(*self, rhs: Self) {
    self.x += rhs.x;
    self.y += rhs.y;
}

fun toString(v: Self): String {
    let s = String::new().sprintln(
        "Vec2: {x: %v, y: %v}",
        v.x,
        v.y
    );

    s
}

// when using file-level impl blocks, you may only have one per file. having multiple blocks is a syntax error.

// Vecs.rvr
impl Vec2;

... code ...

impl Vec3; // ERROR


    
```

# Methods

built-in types, and user-defined types can both have methods assigned to them.
methods are functions that are in the type's namespace.

```rs

struct Vec2 {
    x, y: int;
}

impl Vec2 {
    // instance method
    fun add(*self, rhs: Self) {
        self.x += rhs.x;
        self.y += rhs.y;
    }

    static fun toString(v: Self): String {
        let s = String::new().sprintln(
            "Vec2: {x: %v, y: %v}",
            v.x,
            v.y
        );

        s
    }
}

let x = Vec2(10, 10);
let y = x.neg();  // mutates x
let z = x:add(y): // does not mutate x
let w = Vec2::add(z, y); // similar to calling z.add(y);    
```

# Using statement

```rs

// the using statement brings items from a specific namespace into the current scope:

struct NPC {
    pos: Vector2;
    health: i32;
    name: string;
}

fun print_npc_pos(npc: NPC) {
    io::println("%v is at pos (%v, %v)", npc.name npc.pos.x, npc.pos.y);
}

fun print_npc_pos(npc: NPC) {
    using npc;

    io::println("%v is at pos (%v, %v)", name pos.x, pos.y);
}

// this only applies when the current scope does not have items with the same name as items from
// the used namespace.
// if there is a name clash, the full qualified name must be used

let x = 10;
let v = Vector2(40, 30);
using v;

x += 10; // this uses the variable x.
v.x += 10; // this uses the vector variable.


// using can also just be directly used on struct fields

struct NPC {
    using pos: Vector2;
    health: i32;
    name: string;
}

fun print_npc_pos(npc: NPC) {
    io::println("%v is at pos (%v, %v)", npc.name npc.x, npc.y);
}

// you can of-course still access the original field directly too:
let n := NPC.{ .pos = .{ 10, 20 } };
n.x += 20;
io::println("pos: %v", n.pos); // { 30, 20 }
    
```

# Generics

river supports generics / polymorphism via parameterized types and procedures:
```rs
struct Arr[$T: typeid] {
    data: [*]T,  
};

impl Arr[$T: typeid] {    

    fun new (size: isize, data: [*]T = null, alloc := context.allocator ): Self {
        let x: Self = Self{};
        x.data = alloc.new(T, isize);

        if data != null {
            mem.copy(x.data, data);
        }

        return x
    }

}

type ArrInt = Arr[int];

let x = ArrInt::new(10);

let y = Arr[f32]::new(24);

// generic procedures can be constrained to only using the specializations of a generic struct like so:
fun pushArray[T, A](arr: A, item: T) where A:Arr { ... }

let x = Arr[int]::new(10);
pushArray<int>(x, 5);
 
```

# Optionals

```c

struct File {...}

fun read_file(path: string): File? {
    ...
}

let f = read_file("foo.txt");
if let Ok(v) = f {
    println("{}", f.data);
}

let f = read_file("foo.txt")?; // unwraps implicitly, panics on error at runtime
println("{}", f.data);
 
```


# Traits

traits model shared behaviour

```rs

trait Stringify {
    fun to_string(self): String;
}

struct Vec2 { x, y: int; }

impl Stringify for Vec2 {
    fun to_string(self): String {
        let s = String::fmt_new("Vec2: { x: %v, y: %v }", self.x, self.y);
    }
}

let s = Vec2(10, 20);
s.to_string(); // => "Vec2: { x: 10, y: 20 }"
    
```

# Miscellaneous operators

## Range operator ..
```
    
```

## Function applicator operator ->
```c

// the function applicator is a convenience operator meant to help with readability
// all it does is takes the lhs and inserts it into the function on the rhs as it's parameters.

// consider the following functions:
fun foo(x: int): string;
fun bar(s: string): float;
fun baz(f: float);

// traditionally they'd be called like so:
let x = 10;
baz(bar(foo(x)));
// the -> operator can however make this much cleaner
x -> foo()
  -> bar()
  -> baz();

let (a, b, c) = (1, 2, 3);
a -> bar(b)
  -> baz(c); 
// equivalent to
baz(bar(a, b), c);

someOtherLongFunc(a, b, c) -> someLongFunc();
// eq to
someLongFunc(someOtherLongFunc(a, b, c));

let result_matrix = mat -> mult(mat2) -> transpose();
// as opposed to
let result_matrix = transpose(mult(mat, mat2));

```


# Pattern Matching

# Error handling

# Memory management

# Closures

# Macros / Code Generation

# Type introspection

```rs

// river supports full runtime type introspection

// however, in the interest of smaller executables, most of the introspection information is disabled by default
// by default, this is the following type information avaiable at runtime:
//     - typeid: type name
//     - typeof: base type [ int types, float types, string, char, bool, struct, enum, union, array, slice, tuple, function pointer ]
//     - sizeof: type size
//     - names: only for enums, array of string representation of the enumeration constant. i.e Colour::Red => "Red"
//     - inner: the "inner" or backing type [ array, pointer, slice ]
//     - len: arrays and enums only
//     - firstof: first element, enums only
//     - lastof: last element, enums only

// you can turn on full runtime introspection for types with the `rtti` attribute
#rtti
struct Player {
    pos: vec3;  // note that the types for all the fields must also have reflection enabled on them
    name: string;
    // other fields
}
// the following is the type information available with full rtti enabled
//     - typeid: type name
//     - typeof: base type [ int, float, bool, struct, enum, union, array, tuple, function pointer ]
//     - sizeof: type size
//     - names: only for enums, array of string representation of the enumeration constant. i.e Colour::Red => "Red"
//     - values: only for enums, array of enum values
//     - inner: the "inner" or backing type [ array, pointer, slice ]
//     - len: arrays and enums only
//     - firstof: first element, enums only
//     - lastof: last element, enums only
//     - fields: array of structs that contains a field name ( string ) along with a type ( typeinfo ), for unions and structs only.
//     - methods: array of type methods
//     - params: only for function pointer types, list of function paramaters
//     - returns: only for function pointer types, return value

fun print_type(t: typeinfo) {
    // ...
}

let p := Player.{ .pos = .{10, 30, 20}, .name = "Joe" };
print_type(p.typeinfo); // => struct Player {
                        //     name: pos, val: .{10, 30, 20}
                        //     name: name, val: "Joe"
                        // }

// #rtti attribute parameters:
    // no-privates: exclute private fields
    // names-only: only include field and type names and exclude typeinfo ( struct and union only )
#rtti(no-privates)
struct User {
    name: string;
    private password: string; // excluded
}

// you can also just manually exclude fields from getting rtti embedded for them:
#rtti
struct Game {
    #no-rtti
    renderer: Renderer; // excluded
    world: World; // included
}

// rtti can also be completely disabled for a type using the `no-rtti` attribute
#no-rtti
struct Foo { ... }

// you can also just simply set a compiler flag that automatically sets the rtti level
// for every single type in the package
// rvr compile src --rtti:[all|none|all-ex|none-ex]
// - all: enables rtti for all types in the package
// - none: disables rrit for all types in the package
// - all-ex: enables rtti for all types in the package except the manually marked ones
// - none-ex: disables rtti for all types in the package except the manually marked ones
```

# C FFI
```c
// use C types
cstr, cvoid, *cvoid

// import functions from C
foreign {
    fun malloc(size: isize): *void;
}

// export functions for C ( pretty naming )
#extern("add_ii")
fun add(x, y: int): int;
#extern("add_ff")
fun add(x, y: f32): f32;

    
```

# Attributes
```
- extern
- rtti: enable rtti for type
- no-rtti: disable rtti for type
- discard: allow discarding function return value
- ignore-error: allow ignoring error value ( requires error to be the last value returned )
    
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

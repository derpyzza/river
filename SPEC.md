# Introduction
This is the language spec for river.
for now this is more of a sketchpad for features, but it'll get leaner and cleaner with each commit!


considerations:
    null safety
    error handling
    interfaces

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
| raw      | mark a union as an untagged union          |
| let      | declare an immutable variable              |
| var      | declare a mutable variable                 |
| const    | declare a compile-time constant            |
| type     | declare a new type                         |
| alias    | alias a symbol to another                  |
| if       | if expression                              |
| else     | else expression                            |
| then     | then branch in if expression               |
| for      | for loop                                   |
| in       | get item from iterator                     |
| while    | while loop                                 |
| do       | do branch in inline for/while loops        |
| repeat   | repeat loop                                |
| until    | until conditional for repeat loop          |
| break    | break loop                                 |
| continue | continue to next iteration of loop         |
| defer    | execute expression at the end of the scope |
| switch   | switch expression                          |
| case     | case branch for switch expression          |
| import   | import a module                            |
| module   | declare a module                           |
| as       | alias an import module | type cast         |

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
| *        | multiplication           | binary | infix     |
| /        | division                 | binary | infix     |
| %        | modulus operator         | binary | infix     |
| -        | negation                 | unary  | prefix    |
| <        | less than                | binary | infix     |
| >        | greater than             | binary | infix     |
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
| <=       | less than or equal to    | binary | infix     |
| >=       | greater than or equal to | binary | infix     |
| not      | boolean not              | unary  | prefix    |
| and      | boolean and              | binary | infix     |
| or       | boolean and              | binary | infix     |
| as       | type cast                | binary | prefix    |
| in       | check item in collection | binary | infix     |
| []       | array index              | unary  | circumfix |
| ..       | range                    | binary | infix     |
| @        | pointer deference        | unary  | prefix    |


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


# Identifiers and Numbers
```
    // identifier
    iden -> [a-zA-Z_] [a-zA-Z0-9_]* ;
    digit -> [0-9] ;

    // number literal
    number -> float | integer;

    // decimal exponent
    dec_exp -> [eE] [-+]? integer ;
    // binary exponent
    bin_exp -> [pP] [-+]? integer ;

    float -> 
        digit* ( '.' digit+ )? dec_exp? [fFdD]?
        | hex_literal* ( '.' hex_literal+ )? bin_exp? [fFdD]? 
    ;

    integer ->   
        (
            digit* ( ['_] digit+ ) dec_exp?
            | hex_literal bin_exp?
        		| octal_literal
        		| binary_literal
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

# Modules and packages
river bundles up it's code files into logical units called modules.
a river program can be made up of multiple modules, and a module can be made up of multiple `.rvr` files.

files in the same module can share code between each other without explicitly importing the code.
that is to say, if a file `fooA.rvr` and a file `fooB.rvr` share the same module, then they also share the same namespace, and do not have to `import` code from each other.

modules come in two forms: directory modules and file modules.
by default, every file inside a directory is considered to be a single self-contained module.

however, if a directory has a `mod.rvr` file, then that directory and every file within the directory is considered to be the same module.
a directory may not have more than one module or `mod.rvr` file at a time in this way.

a directory that has a `mod.rvr` file in it, which will be referred to as a directory module from here on out, may itself contain subdirectories.
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


## File imports 

```c

// in the C backend, `import`ed variables and functions just get pasted in as forward
// declarations.
// then later, the linker compiles the definition in with the parent module's .o file

// import modules with the import keyword
import std::io;

fun 
main() {
    io.println("Hello!");
}

// import name aliasing
import std::math as m;

m.abs();


import std::io;
using io; // expands io into current scope.

fun
main() {
    println("Hello");
}

```

# Constants
```
    const_def -> "const" iden "=" expr ";" ;
```

you can declare compile time constants with the `const` keyword.
constants are akin to `#define` in C, they alias a symbol to an expression.
```c

const PI = 3.1415;
const TWO_PI = PI * 2; 

```

# Variable declaration & definition

```
    var_def -> 
          ( "let" | "var" ) iden ( ":" type )? "=" expr ";"
        | ( "let" | "var" ) iden ( ',' iden )* ( ":" type )? "=" expr ( "," expr )* ";"
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
let w; // illegal
let w: int; // legal


let
    x, y, z, w: int,
    a, b, c, d: float,
    q, w, e: string
    ;


// variables are declared to a zero-value be default. each primitive type has it's own zero value
let x: int;    // x = 0
let x: bool;   // x = false
let x: string; // x = ""

// if it is desired for a variable to be uninitialized, it can be specified using the `undef` keyword:
let x: int = undef;

// You can assign multiple variables at once:
let x = 10, z = "string" , y: float = 3.14;

// you may also declare multiple variables at once:
let x, y: int, z: float;
// if initializing variables however, they must all be manually marked with types:
let x = 10.4, y: int = 10, z = "string"; // illegal
let x: float = 10.6, y: int = 10, z: string = "string"; // okay

// unless they're the same type:
let a = 10, b = 20, c: int = 30;
```

c output:
```c

// let x = 10;
int x = 10;

// var x = 10;
int x = 10;
    
```

# Pointers

```rs

// pointers are defined as such:
let name: *type = &non_pointer_variable;
// pointers are dereferenced as such
let name: type = @pointer_variable;

// pointers can have up to 8 levels of indirection:
let name: ********type = value; // pointer to pointer to pointer to ... 8 times to a type of value.

let ptr: ********int ; // pointer to pointer to pointer to ... 8 times to an int.

```
pointer arithmetic is disallowed. the only operations a pointer has are the address-of and deference operations.
Null pointers also do not exist. instead, nullable pointers are wrapped up in an [`Optional`](#Optionals) type.

c output:
```c

// let variable = 10;
// let ptr = &variable;
// let otherVar = @ptr;
int variable = 10;
int *ptr = &variable;
int otherVar = *ptr;
```

# Arrays

```rs
// arrays are defined as such:
let name: [size] type = value;
let arr:  [6]int = {1, 2, 3, 4, 5, 6}; // array of six integers

// arrays have their length encoded into them:
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size N will have N elements in it but the final element will have 
// the index [N-1], as indices begin at [0]
arr.len(); // => 6

// array types must always contain the size
let name: []type = value; // => ERROR, missing array size
// but for array literal definitions you can simply just put a _ to infer the length:
let arr: [_]u8 = {3, 6, 7, 2, 6}; // => Okay ✅
let arr: [_]u8; // Error: Must provide a default array

let arr = [5]u8{0, 1, 2, 3, 4}

// you can initialize items in specific locations in an array using the following shorthand:
let arr: [10]bool = {
    3 = false,
    6 = true,
};

// you can initialize an array or a portion of an array with a value using the ... operator
let buf: [512]u8 = {... = 0x42};
let foo: [10]u8 = {1, 4, 6, ... = 32};

// similarly, you can also assign elements over ranges:
let mem: [512]u8 = {
    0x00..0xFF   = 0x42,
    SPECIAL_BYTE = 223,
    OTHER_BYTE   = 243,
    FLAG_BYTE    = Flag.Foo | Flag.Bar;
    0x120..0x200 = 0x00,
};

// for multi-dimentional arrays
let map: [10][10]f32 = {...0};

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

slices are views into contiguous elements in memory, kinda like an array, but they point to memory rather than contain it directly
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

except instead of doing all that, it's just a self-contained type with a len and a pointer part.
basically just a fat pointer.
the length of slices are known at run-time, rather than at compile time like with arrays.

```rs

let arr: [_]u8 = [4, 6, 7, 8, 3, 7];
let slice: [*]u8 = arr[3..5]; // => [8, 3, 7], len 3

let mem: [*]u8 = mem.alloc(u8, 100);
mem.len(); // => 100

// like with regular pointers, pointers to arrays can have up to 8 levels of indirection:
// NOTE: Maybe this is like, stupid as heck???
let arr: ********[6]int; // pointer to pointer to pointer to ... 8 times to an array of 6 ints;
let arr: ********[6]********int; // pointer to pointer t ... 8 times an array of 6 pointers to pointers to ... 8 times to an int;

```

c output:
```c

// let arr: [_]u8 = [4, 6, 7, 8, 3, 7];
// let slice: [*]u8 = arr[3..5];
uint8_t arr[6] = {4, 6, 7, 8, 3, 7};
RVR_SLICE(u8) slice = RVR_SLICE_FROM(arr, 3, 5);
    
```

# Strings

river supports three types of string literals:
- normal literals
- raw literals
- multiline literals

```rs
// the string type in river is an alias of [_]char
let s: string = "hello world";
s.len(); // 11

let sp = s.split(6)[0]; // "hello"
```

river supports multiline strings too:
```c
let str =
    \\ hi hello
    \\ this is a multiline string yaaay
    \\
    \\     this is an indented line
    ;
```

however, if that's unsavoury for you for whatever reason, then river still supports C's style of automatically concatenating string literals next to each other.
though of-course, in this case you'd have to provide formatting and newlines yourself.

```c
let str =
    "hello\n"
    "\n"
    "this is a multi-line string\n"
    "\n"
    "\tthis is an indentation woo\n";    
```

# Control flow

## If expression
```c

if expression {
    // do stuff
} else if other_expression {
    // do other stuff
} else {
    // do yet more stuff
}
// if statements in river are actually expressions, and can return values
let x = if expression {
    value
} else expression {
    value
};

// if expressions don't need parenthesis for their condition unlike in C.
// instead, the body MUST either be contained in a block, or use the `then` keyword
if bool { something() } else { something_else() }
if bool then something() else something_else()

// when if statements are being used as an expression, i.e they're expected to produce a value, you MUST provide an else clause in case the if check fails:
let x = if false then 10; // ERROR: x never gets initiated :(
let x = if false then 10 else 20; // OK

let x = 10 if false else 20;

let x = if bool then 10 else if other_bool then 11 else 23;


// else expressions can be used on their own to provide fallback values in case of failure:
let config = read_config() else default_config;
// here the else expression only gets triggered if read_config() results a falsey value.
```

c output:
```c

// let x = if someCheck() { 10 } else { 20 };
int x;
if (someCheck()) {
    x = 10;
} else {
    x = 20;
}

// let config = read_config() else default_config;
Config config;
RvrMaybe _config = read_config();
if (_config.status == RVR_MAYBE_OK) {
    config = (Config)_config.ok;
} else {
    config = default_config;
}

    
```

## Switch expression
    
```c

// switch statements are also actually expressions, and can pattern match
switch x {
    case do ...,
    case do {...},
    // cases don't have implicit fallthrough and a fallthrough must be explicitly forced via use of the `fallthrough` keyword
    case do {
        ...
        fallthrough
    },
    // cases can be chained together
    case | case | case do {...},
    // default catch case
    default do {...} 
}

// when switch cases are being used as expressions, they too must either exhaustively match all cases or provide a default fallback.

fun x_to_string(x: X): string =
    switch x {
        X_XX    do "XX",
        X_XXX   do "XXX",
        X_XXXX  do "XXXX",
        default do "bleh"
    };
```

## While loop
all the loops are statements, and not expressions.
    
```c

// while loops are pretty normal
while expression { ... };
while expression do expression;

```

## Repeat-Until loop
    
```c

// repeat..until loops repeat an expression until the condition is held true
// they're kinda like an inverse do while, where do..while repeats until a condition is false, repeat..until repeats until a condition is true
repeat {

} until expression;

let x = 0;
repeat x++ until x == 10;
```

## For loops
    
```rs

// river has the classic C style for loops available to use
for init;cond;inc {...}
for init;cond;inc do expression;

// NOTE: should `let` be ignored here? since x is literally being mutated in the for loop? or should the `let` here mean that it's illegal for it to be mutated *within* the loops body only?
for let x = 0; x < arr.len; x++ {
    io.println("x is: {}", arr[x]);
}

// this is better written using a for..in loop:
for let x in arr {
    io.println("x is: {}", x);
}

for let x in [0..10] do io.println("{}", x*x);


for item in list do expr

// the for-in loop uses pattern matching to destructure collections
for let index, value in list {
    do_something();
}

```

# Custom types and symbol aliasing

```c

// You can define distinct types with the use of the 'type' keyword:
type Celcius = double;

// types defined by the `type` keyword are distinct from their base types:
let temp: Celcius = 35.0;
let f: double = temp; // illegal, cast via `as double`
let f: double = temp as double; // ok 👍
```

# Structs 

```c

// You can wrap up several datatypes into a big datatype called a struct, just like in C 
type Vector3 = struct {
    var x, y, z: f32;   
}

// struct values must declare their mutability upfront with the var and let keywords:
type Entity = struct {
    let id: string;
    var pos: Vector3;
    var health: 
}

var e: Entity = Entity.{
    .id = "newEntity",
    .pos = Vector3.{10, 10, 10},
    .health = 10,
    };

e.health = 20; // OK ✅
e.id = "bleh"; // Not OK ❎, will error at compile time

// however, if a struct variable is declared with `let`, you cannot change it's fields at all

let e: Entity = Entity.{
    .id = "newEntity",
    .pos = Vector3.{10, 10, 10},
    .health = 10,
    };
e.id = "bleh"; // Not OK ❎, will error at compile time
e.health = 20; // Also Not OK ❎

// struct field tags
// you may tag struct fields with a string which attaches meta info to the struct
type GameObject = struct {
    var pos: Vec3 `json:"pos"`;
    var transform: Mat4 `json:"transf"`;
    var active: bool; // untagged
}

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

let (x, y, z, w, e, t) = (13, 45, 35, ... = 3); // w, e, t == 3, 3, 3

```

# Enums 
```go

// similar to C's enums, except with their own namespace
type Colour = enum {
    Red,
    Blue,
    Green,
    White,
    Yellow,
    Brown,
}

type MnMs = enum {
    Red,
    Blue,
    Green,
    Yellow,
    Brown,
    Orange,
}

let x = Red; // => Error: Assigning variable <x> to unknown value <Red>;
let x = Colour.Red; // => compiles
let y: Mnms = Red; // => Also compiles

// you can assign values to your enumarations
// enums are ordered, so if you leave unassigned gaps they just take on the values of the last assigned value + N where N is the distance from the last assigned value
type Flags = enum {
    Clear = 1<<0,
    Stop  = 1<<1,
    Start = 1<<2,
    Idk   = 512,
    Other, // 513
}

// you can specify the backing type of an enum ( integer only )
type Something = enum u8 { Something, OtherThing }

// enums with explicit backing types can be used in place of the backing type:
fun foo(num: u8) {...}

foo(Something.OtherThing); // okay

// otherwise you'd have to manually cast:
fun bar(num: u16) {...}

bar(Something.OtherThing); // error
bar(Something.OtherThing as u16); // OK 👍

// you can get the number of enumerations within an enum by using the count function:
for c in #count(Colour) do something(c);

// enums can be tagged with a string
type Colour = enum {
    Red   "colour_red",
    Blue  "colour_blue",
    Green "colour_green",
}

// enums are concatenative:
type OperatorTag = enum {
    Add "+",
    Sub "-",
    Mul "*",
    Div "/",
}

type KeywordTag = enum {
    Return "return",
    Func "func",
    If "if",
    Else "else",
}

// TokenTag now contains the enumerations for OperatorTag and KeywordTag within it's namespace
// while KeywordTag and OperatorTag both maintain their own separate namespaces
type TokenTag = enum : OperatorTag, KeywordTag {}


// extended enums can have their own fields too
// the enumerations are placed in the following order:
// the enum's main body comes first, then all of it's extensions get inserted in after the main body, one by one in order.
// i.e in this case, EntityKind.Null is 0, EntityKind.Other is 2, and then NpcKind, EnemyKind, and ItemKind get placed in order after that.
type EntityKind = enum : NpcKind, EnemyKind, ItemKind {
    Null,
    Player,
    Other,
}

// you may manually lay out the order of extended enums, if needed:
type TokenTag = enum : OperatorTag, KeywordTag {
    None = 0,

    ::KeywordTag,
    ::OperatorTag,

    Eof
}

// for convenience's sake you can access the first and last elements of an enum directly, including nested ones
#first(TokenTag.KeywordTag); // TokenTag[.KeywordTag].Return
#last(TokenTag.KeywordTag); // TokenTag[.KeywordTag].Else

// TODO: figure out value clashes such as this:
type Colour = enum { Red = 2 }
type JellyBean = enum { Strawberry = 2 }
type IdkSomething = enum : Colour, Jellybean {} // both Colour.Red and JellyBean.Strawberry have the same value. error? or maybe offset the values based off the enum.
// offsetting the value does however seem like it might be bug-prone?

```

# Unions

```c

// unions are pretty standard, only one item can be active at a time, the size of the union is the same as that of it's largest member
// unions in river are discriminated by default
type Literal = union {
    None,
    Int: int,
    Float: f64,
    String: std::String,
    Char: char
};

let intLit = Literal.Int(19);

let v = someLiteralFunction();

switch v {
    Int(v)    => printf("Int: {}", v);
    Float(v)  => printf("Float: {}", v);
    String(v) => printf("String: {}", v);
    Char(v)   => printf("Char: {}", v);
}

type Maybe(T) = union {
    None,
    Some: T,
}

type Result(T, E) = union {
    Error: E,
    Ok: T,
}

fun readFile(path: string): Result(File, FileError) {
    let f = file.open(path);
    switch f {
        Error(e) => {
            
        },
        Ok(f) => {}
    }
}

// unions can be untagged too, if desired:
type Thingy = raw union {
    Int: int,
    Float: float,
}

```

# In expression

```go

type Colour = enum {
    Red, Blue, Green, Orange, Magenta, Cyan, Yellow, Black, White
}

let x: Colour = getColour();

if x in [ Colour.Red, Colour.Yellow, Colour.Blue ] {
    io.println("primary colour!");
} else {
    io.println("other colour: {}", x);
}
    
```


# Functions 

```
    function_def -> "fun" %id ( "(" params_list ")" )? ( ":" type )?
        (("=" expr ";") | block)
        ;   
```

```c

// NOTES:
//
// * Function return types MUST be explicit, unless using = syntax

fun sub ( x, y: int ): int {
    return x - y; // returns x - y
}

// functions implicitly return the last expression in the body:
fun make_point (x, y: int): Point {
    Point.{x, y} // implicitly returned
}

// for one-liner functions you can just use an expression or a statement in the body.
// the return type is inferred:
fun sub ( x, y: int ) = x - y;

// function types can be declared as such:
// this is a function that has a parameter of type int and returns a value of type bool
type func = fun(int): bool;

fun smth(i: int, callback: func): bool = not callback(i);

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

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
fun example_func( val: float , func: fun(float, int) ): u32 = {
    func(val, 20); // => calls the passed-in function
}

// though it's nicer to just give the return functions a type alias
type callback = fun(int, float): bool;

```

# Methods

built-in types, and user-defined types can both have methods assigned to them.
methods are functions that are in the type's namespace.

```c++

type Vec2 = struct {
    x, y: int,
}

fun Vec2::add(self: Self, rhs: Vec2): Vec2 = { ... };
fun Vec2::neg(self: *Self): Vec2 = { ... };

let x = Vec2(10, 10);
let y = x.neg();  // mutates x
let z = x:add(y): // does not mutate x
let w = Vec2::add(z, y); // similar to calling z.add(y);
    
```
# Optionals

```c

type File = struct{...};

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

# Pattern Matching

# Error handling

# Memory management

# Traits

# Generics

river supports generics / polymorphism via parameterized types and procedures:
```rs
type Arr<T> = struct {
    data: [*]T,  
};

fun newArray<T>( size: isize, data: [*]T = null, alloc: Allocator = context.allocator ): Arr<T> {
    let x: Arr<T> = Arr<T>{};
    x.data = alloc.new(T, isize);

    if data != null {
        mem.copy(x.data, data);
    }

    return Arr<T>
}

let x = newArray<int>(10);

// generic procedures can be constrained to only using the specializations of a generic struct like so:
fun pushArray<T, A>(arr: A, item: T) where A:Arr { ... }

let x = newArray<int>(10);
pushArray<int>(x, 5);

    
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

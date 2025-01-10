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
    %iden -> [a-zA-Z_] [a-zA-Z0-9_+/\-?!%^*\']* ;

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
            | <hex_literal>
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
    123u12     // u12 ( arbitrarily sized integer ) literal
    123i8      // explicit i8 literal
    123i16     // explicit i16 literal
    123i32     // explicit i32 literal
    123i64     // explicit i64 literal
    234uz      // explicit usize literal
    424iz      // explicit size literal
    133f       // explicit f32 literal
    234d       // explicit f64 literal
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

- anyptr:   A pointer type that can be cast down to any pointer type. equivalent to C's void*
- bool:     boolean type, true | false
- char:     alias of u8
- f32:      Floating point number
- f64:      Double precision floating point number
- f80:      10-Byte floating point number
- int:      signed cpu word sized integer
- iN:       signed N bit integer where n is non-negative : eg u7 is a 7-bit integer
- i8:       signed 8 bit integer
- i16:      signed 16 bit integer
- i32:      signed 32 bit integer
- i64:      signed 64 bit integer
- uint:     unsigned cpu word sized integer
- uN:       unsigned N bit integer where n is non-negative : eg u7 is a 7-bit integer
- u8:       unsigned 8 bit integer
- u16:      unsigned 16 bit integer
- u32:      unsigned 32 bit integer
- u64:      unsigned 64 bit integer
- size:     Signed pointer sized integer
- usize:    Unsigned pointer sized integer
- string:   Distinct type of static array of u8s
- void:     has only one value, void, used for expressions that return nothing

River has the following literal types:

- null:     memory address 0x0. only applicable to pointers. compiles down to (void*)0;
- numbers:  signed/unsigned integers, floating point numbers,  
- string:   "string"
- true:     just the value true. compiles down to the expression (0==0)
- false:    just the value false. compiles down to (0!=0);

## Reserved Keyword list 

// type stuff
anyptr bool char f32 f64 f80
usize size string null undef 
uint int
u8 u16 u32 u64 
i8 i16 i32 i64
true false
void

// declaration stuff
struct enum union fun let const mut type alias

// control flow stuff
if else for while break continue goto return defer label
switch

// 
sizeof typeof alignof

new delete macro

impl interface import as from with is
extern // ffi
 

# Variable Definition & Declaration

```
    var_def -> 
          ( "let" | "var" ) %id ( ":" %id )? "=" expr ";"
        | ( "let" | "var" ) %id ( ',' %id )* ( ":" %id )? "=" expr ( "," expr )* ";"
    ;
    
```

```rs

// variables are declared with the 'let' keyword
let x: int = 45;
x = "str"; // illegal

let f = fopen("path/to/file").unwrap();

let x, y = 10, 20;

let x, y: int, z: string = 10, 23, "34";

var x, y, z = 12, 2, false;

let x, y, z, w, e, t = 13, 45, 35, .. _;

let q: [5]u8 = [0, .. 1];

let q, w, e, r: int = 12, 23, 35, 4;

let x, y, z;

f: file = fopen("path/to/file").unwrap();

// let-introduced values are immutable by default
// And also private to their scope

// to declare a mutable variable use the mut keyword:
let mut x = 0;
x = 10; // legal

let mut x, mut y, mut z;

// you can declare variables without initializing them.
// However, when declaring a variable, you must provide it's type
// i.e using let is illegal
let y, z, w; // illegal
let y, z, w: int; // legal

// You can assign multiple variables at once:
let x, y, z = 10, 20, 30;
// Even if the types are separate:
let x, y, z = "X", 230, false;


// variables are non-nullable by default
// to have a nullable variable, it must be explicitly marked as such
// by prepending a '?' to the variable name
// nullability is defined as a datatype
let name: ?string = get_name();

let x = 10;
let y: ^int = &x; // & is the address of operator, pointer types are expressed by adding a caret before the type name

// pointer dereferencing
let q = ^c, 
    w = ^v,
    e = ^b;
```

# Pointers and arrays

```c
// pointers are defined as such:

let name: ^type = value;

// pointers are marked with the caret or hat '^' operator.
// pointers can have up to 8 levels of indirection:

let name: ^^^^^^^^type = value; // pointer to pointer to pointer to ... 8 times to a type of value.

let ptr: ^^^^^^^^int ; // pointer to pointer to pointer to ... 8 times to an int.

// unlike in C, pointers and arrays are distinct types
// however, they are both defined before the type:

name: [size]type = value;
arr: [6]int = [1, 2, 3, 4, 5, 6]; // array of six integers

// array types must always contain the size

[]type name = value; // => ERROR, missing array size

// arrays can be pointed to too:

arr: ^[6]int; // pointer to an array of 6 ints
arr: [6]^int; // array of 6 pointers to ints
arr: ^[6]^int; // pointer to array of 6 pointers to ints

// like with regular pointers, pointers to arrays can have up to 8 levels of indirection:
^^^^^^^^[6]int arr; // pointer to pointer to pointer to ... 8 times to an array of 6 ints;

^^^^^^^^[6]^^^^^^^^int arr; // pointer to pointer t ... 8 times an array of 6 pointers to pointers to ... 8 times to an int;
    
```

# Functions 

```

    function_def -> "fun" %ID ( "[" generic_list "]" )? ( "(" params_list ")" )? ( "->" %ID )? "=" expr ";" .   

```


```c

// Code blocks, including functions, support implicit returns
// the last expression within a block is returned
fun sub ( x: int, y: int ) -> int {
    x - y // returns x + y
}

// the above function can also be written as:
fun sub ( x, y: int ) -> int = x - y;

fun sub[T: type where T is subtractive](x, y: T) -> T = x - y;

fun div(num, div: int) -> int where div > 0 = num / div;

fun fclose(^File fp) where fp != null = { ... };

// functions without parameters can be omit the parenthesis
fun do_stuff = {
    // do something
}

fun add 10; // error! expected '=>' or '=', got Int_Literal!
               // help: try adding a '=>' before the number '10'
               //     | add '=>' 10;
               //     |~~~~~~^^~~~~~

// function types can be declared as such:
// this is a function that has a parameter of type int and returns a value of type bool
type func = fun(int) -> bool;

fun do_something() -> foo, (bar | null) = {
    
}

// Functions support default parameter values
// If an argument is not passed for a particular parameter,
// the default value is used instead
fun Colour_from_rgba( r, g, b: int, a: int = 255 ): Colour = {
    Colour {r, g, b, a}
}

// Arguments with default parameters can be omitted
Colour_from_rgba(125, 125, 125); // param 'a' => 255;
Colour_from_rgba(125, 125, 125, 125); // param 'a' => 125;

// Function arguments can be named when calling functions
// Take the following function for example:
fun draw_rect_pro ( x, y, width, height: int, rotation: int, col: Colour ) -> Rectangle = { /*...*/ }
// You can call the function by naming the individual arguments
let rect = draw_rect_pro(
    x = 20,
    y = 30,
    width = 240,
    height = 360,
    col = (10,10,10,255));
// The advantage of named arguments is that you can rearrange the order in which you pass them:
draw_rect_pro(
    width = 240,
    height = 360,
    x = 20,
    y = 30,
    col = (10,10,10,255)); // perfectly valid

// functions can have multiple return values by utilizing tuples
let read_file(string path) -> File, string = {
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
fun consume = scanner.char += 1; // note, how do you decide whether or not this function returns a value, without a hint? the rhs of this function is an assignment expression, what does that evaluate to?
fun peek = scanner.token[scanner.char + 1];

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
fun example_func( float val, fn func(float, int) ) -> u32 = {
    func(val, 20); // => calls the passed-in function
}

// functions can return other functions
fun func() -> fun(bool) -> int = {
    // functions can be defined inline as closures
    return fun(bool x) -> int {
        if x {
            true
        } else {
            false
        }
    };
}

// though it's nicer to just give the return functions a type alias
type callback = fun(int, float) -> bool;


// function contracts

int div(int x, int y) => {
    ensure x > 0;
    ensure y > 0; // => panics if either of these are wrong

    ensure;
    request;

    let z = x / y;
    ensure z != 0; // for some reason. just as an example
    return z;
}

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



```

# Control flow

```c

// all the following control flow structures are expressions rather than statements

let x = if bool { 10 } else if other_bool { 11 } else { 23 };
let x = if (bool) 10 else if (other_bool) 11 else 23;

if x == "string" {
    do_something();
}
if ( x == "string" ) {
    do_something();
}

// optional 'then' keyword for one-liners
if x then y else z;
if x then y;

if x {

}

if x then {

};

if let PATTERN = EXPR {

}

Colour x = (255, 0, 255, 0);

if let x = (255, 0, 0, 0) then printf("x is pure red") else printf("x is not pure red"); 


string x_to_string(X x) => switch x {
    case X_XX => "XX",
    case X_XXX => "XXX",
    case X_XXXX => "XXXX"
};




if ( expression ) {
    // do stuff
} else if ( other_expression ) {

} else {
    // do yet more stuff
}

while ( expression ) {
    // do stuff
}

do {

} while expression;

if x then y else if z then w else a;
if x then y else while z do do_something();

for each x in y => do_something();
while x => do_something();

for x; y; z do do_something(); 

for x;y;z => do_something();

for each x in range(0..=10) {

}

for each item in list {

}

for each (id, item) in list {

}

// C style
for let x = 0; x < 10; x++ {

}

// switch expressions cannot be inlined, unlike if, for and while expressions;
switch expr {
    case => expr,
    case => {...},
    case | case | case => {...},
    _    => {...}, // default case
}

```



# structs 

```c

// You can wrap up several datatypes into a big datatype called a struct, just like in C 
type Vector3 = struct {
    f32 x, y, z;
};

// Unlike in C, you don't need to typedef structs to avoid the struct keyword in declarations
// Also unlike in C, struct fields cannot be anonymous
Vector3 v = Vector3 {.x = 10.f, .y = 10.f, .z = 12.f};

type Vector4 = struct {
    // Fields are non-nullable by default
    f32 x, y, z;
    // However, fields can be nullable
    // appending a ? to a variable name makes it nullable
    // Like pointers, nullability is a property of a variable, not a datatype
    f32 w?;
};

type User = struct {
    // Struct fields can have default values
    str username = "Admin";
    u16 pin = 0000;
};

type Core = struct {
  window: struct {};
  input: struct {};
  _: union {};
  shader: shader | u32;
};

let admin: User;
printf("user.name = %s", admin.username); // => "user.name = Admin"
admin.name = "Administrator";
printf("user.name = %s, user.pin = %i", admin.username, admin.pin) // => "user.name = Administrator, user.pin = 0000"; 

// NOTE: Do structs really need methods?
// what if instead of "true" methods there was Uniform function call syntax instead?

fun add(x, y: int) -> int = {
    x + y
}

let x = 10;
let y = 30;
let z = x.add(y); // => 40
let w = z.add(y).add(x); // => 80. equivalent to add(z, add(y, x));


// generic structs

type DynArray = struct<$T: type> {
    id, max: size;
    data: ^T;
};


```

# Tuples
```c 

// tuples are like structs, except they don't have named parameters
(int, string) tup = (23, "John");
tup.0; // 23
tup.1; // "helli"

// you can destructure a tuple into multiple variables.
let (age, name) = tup;
// do note that doing so requires the use of the `let` keyword.
// You cannot define the type explicitly:
(int, string) (age, name) = x; // illegal 
// as the type signature expects a tuple of type (int, string)
// but it instead gets individual two variables, `id` and `name` instead.

(int, string) get_person(int id) {
    return (people[id].0, people[id].1);
}

let person = get_person(id);

type Colour = (ubyte, ubyte, ubyte, ubyte);
Colour red = (255, 0, 0, 255);

```

# Enums 
```c 

// similar to C's enums, except with their own namespace

enum Colour {
    Red, Blue, Green, White, Yellow, Brown
};

enum MnMs {
    Red, Blue, Green, Yellow, Brown, Orange
};

let x = Red; // => Error: Assigning variable <x> to nknown value <Red>;
let x = Colour.Red; // => compiles
let y = MnMs.Red; // => Also compiles

// enums can act as discriminated unions:
enum Literal {
    Int(long),
    Float(double),
    String(std::String),
    char(char)
}

type Literal = union {
    Int(long),
    Float(double),
    String(std::String),
    char(char)
};

// i should have both normal unions but also ad-hoc inline unions like
// int | string, file | Error, etc.
type Vec3 = bare union {
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

// Types can also be made up of literals
// This way, a type can represent legal values and guard against illegal ones
type curry -> "Katsu" 
            | "Vegetable" 
            | "Chicken";

// ( BBQ curry?! )
curry bbq_curry = "BBQ"; 
// => Error: Invalid value "BBQ style" for type <curry>
//  | type union <curry> only supports the following values:
//  | {"Katsu", "Vegetable", "Chicken"}

curry butter_chicken = "Chicken"; // => works just fine

type age = 0 < int < 130;

```

## File imports 

```c

// in the C backend, `import`ed variables and functions just get pasted in as forward
// declarations.
// then later, the linker compiles the definition in with the parent module's .o file

// import modules with the import keyword
import std.io;

int 
main() {
    io::println("Hello!");
}

import std.math as m;

m::abs();


import std.io;
using namespace io;

int 
main() {
    
}



```



# Namespaces
```c

// the namespace keyword declares a new namespace
namespace Vector3 {
    Vector3_s add(Vector3_s x, Vector3_s y);
    Vector3_s sub(Vector3_s x, Vector3_s y);
}

// using a function from a namespace
Vector3:add();
// or
// declaring a scoped namespace
{
    using namespace:Vector3;

    add(x, y); // Vector3::add();
    sub(y, x); // Vector3::sub();
}
add(x, y); // => Error: call to undefined function <add>

```

# Interfaces 
```cpp

// Interfaces define polymorphic functions
interface debug {
    string to_string(T t);
    void debug_print(T t);
}

struct Vector3_s {
    f32 x, y, z;
}

impl debug for Vector3_s {
    
    string to_string(Vector3_s t) => {
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

interface add<T> => where T is Numeric {}
interface cat<T> => where T is Array {}

int count_entities(T entities) => where T has Entity {
    for t in entities {
        t.print_name();
    }
}

```

# Arrays & Slices

```c

// river's arrays are almost the same as C's, with some key differences
int[4] numbers = { 10, 20, 30, 40 };
// firstly, arrays have their length encoded into them
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size 4 with have four elements in it but the final element will have 
// the index [3], as indices begin at [0]
numbers.length // => 4
// secondly, you can initialize arrays with default values
int[0;10] numbers; // => array of size 10 with every element being initialized to 0
int[5;8] numbers; // => array of size 8 with every element being initialized to 5

// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
int print_num_array ( int[10] numbers ) { /*...*/ }

^int nums;
print_num_array(nums); // ERROR: Incompatible types; expected type int[10], got ^int instead;

// arrays can be concatenated with the '..' operator
int[5] x = {0, 1, 2, 3, 4};
int[10] y = {-5, -4, -3, -2, -1} .. x; // => {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4}


// A Slice is a referenced subset of another array
str[6] animals = { "cat", "dog", "cow", "sheep", "chicken", "lion" }; 
// Slice of all the farm animals
str[] farm_animals = animals[2..4]; // => { "cow", "sheep", "chicken" };

```

# Strings

```c

```

# Goto 

```c

// Gotos work the same as in C, except labels are more explicit
// Jump labels are required to be marked with the label keyboard

{
  // jump label
  label jumpHere:
    // code block
}

if expr {
    goto jumpHere;
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

# Pointers

```c

// pointers take the following form:
^type ptr = val;

u32 val = 10;
^u32 ptr = val;
@ptr // => dereference pointer, 10
&ptr // => address of val

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

    band bor bixor binot
    or perhaps
    &b |b ~b !b
    b& b| b~ b!

    or maybe, unary
    & | ~ !

// assignment 
+= -= *= /= %=

 ^ ~ & && ||
. , ? : ; ' " = ( ) { } [ ]
@ .. ... -> =>

# Grammar specification

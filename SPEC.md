# Introduction
This is the language spec for river.
for now this is more of a sketchpad for features, but it'll get leaner and cleaner with each commit!

# Comments
```c 

// Double slashes indicate a single line comment 

/*
    A Slash followed by an asterisk marks a multiline comment, 
    which is ended by an asterisk followed by a slash.
    Just like in C!
*/

```

# Data types and literals
```c 

// River comes with several builtin basic data types
// The builtin types are as follows:
/*
 * void     - No type / no value
 * bool     - boolean type / true | false
 * ubyte    - Unsigned 8 bit integer
 * ushort   - Unsigned 16 bit integer
 * uint     - Unsigned 32 bit integer
 * ulong    - Unsigned 64 bit integer
 * byte     - Signed 8 bit integer
 * short    - Signed 16 bit integer
 * int      - Signed 32 bit integer
 * long     - Signed 64 bit integer
 * float    - Floating point number
 * double   - Double precision floating point number
 * isize    - Signed pointer sized integer
 * usize    - Unsigned pointer sized integer
 // maybe replace char with rune for better clarity?
 * char     - A Single utf-8 code point i.e represents a single utf-8 character,
              and is therefore four bytes and not a byte like in C.
 * str      - A statically sized utf-8 string literal
*/

// River has the following literal types:
/*
 *  null        - no value. compiles down to (void*)0;
 *  numbers     - signed / unsigned integers, floating point. 
 *  string      - "string"
 *  true        - just the value true. compiles down to the expression (0==0)
 *  false       - just the value false. compiles down to (0!=0);
 */

 123 // integer
 123.223 // floating point. defaults to double
 123.44f // float
 232.24d // double
 12_232 // same as 12232, underscores help with readability
 0x32def // hex
 0b11010100 // binary
 0o232ac // octal
```

# Variable Definition & Declaration

```c

// The let keyword inferences the type of a variable by whatever the first value is that the 
// variable is assigned to.
// Here it takes on the value of an int.
let x = 45;
x = "str"; // illegal

// Variables are immutable by default
// And also private to their scope

// to declare a mutable variable add the mut keyword before the type name:
let mut int x = 0;
x = 10; // legal

// you can declare variables without initializing them.
// However, when declaring a variable, you must provide it's type
// i.e using let is illegal
let y, z, w; // illegal
int y, z, w; // legal

// You can assign multiple variables at once:
int x = 20, y = 50, z = 40;
// Even if the types are separate:
let x = "X", y = 230, z = false; // perfectly valid, though do note the use of the 'let' keyword 

// variables are non-nullable by default
// to have a nullable variable, it must be explicitly marked as such
// by appending a '?' to the variable name
// Unlike pointers, nullability is a datatype
str? name = get_name();

int x = 10;
^int y = &x;
^int (c, v, b) = &x; // three pointers.

int q = ^c, 
    w = ^v,
    e = ^b;

```

# Functions 

```c

// Code blocks, including functions, support implicit returns
// the last expression within a block is returned
fn int sub ( int x, int y ) {
    x + y // returns x + y
} 

// the above function can also be written as:
fn int sub (int x, int y) => x - y;

// functions without parameters can be omit the parenthesis
fn int do_stuff {
    // do something
}

fn int add 10; // error! expected block or '=>', got Int_Literal!
               // help: try adding a '=>' before the number '10'
               //     | add '=>' 10;
               //     |~~~~~~^^~~~~~

// function types can be declared as such:
// this is a function that has a parameter of type int and returns a value of type bool
type func = int => bool;

// Functions support default parameter values
// If an argument is not passed for a particular parameter,
// the default value is used instead.
fn Colour Colour_from_rgba( int r, int g, int b, int a = 255 ) {
    return Colour {r, g, b, a};
}

// Arguments with default parameters can be omitted
Colour_from_rgba(125, 125, 125); // param 'a' => 255;
Colour_from_rgba(125, 125, 125, 125); // param 'a' => 125;

// Function arguments can be named when calling functions
// Take the following function for example:
fn Rectangle draw_rect_pro (int x, int y, int width, int height, float rotation, Colour col) { /*...*/ }
// You can call the function by naming the individual arguments
fn Rectangle rect = draw_rect_pro(
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
fn (File, str) read_file(str path) {
    // ... file reading logic ...

    return (file, extension);
}


let (file, ext) = read_file("SPEC.md");
if ext == ".md" {
    parse_markdown(file);
}

// In the case of functions that return nullable values, you can unwrap
// the value using the '?' operator:
str name = get_name()?; // => if get_name() returns null then the program panics;

// for one liners without params the parenthesis can be omitted 
fn char consume => scanner.char += 1;
fn char peek => scanner.token[scanner.char + 1];

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
fn u32 example_func( float val, fn void func => (float, int) )
{
    func(val, 20); // => calls the passed-in function
}

// functions can return other functions
fn (bool => (int)) func () {
    // functions can be defined inline as closures
    return fn bool _ (u32 x) {
        if x {
            true
        } else {
            false
        }
    };
}

// though it's nicer to just give the return functions a type alias
type callback = (bool => (int, float));


// function contracts

fn int div(int x, int y) {
    ensure x > 0;
    ensure y > 0; // => panics if either of these are wrong

    ensure;
    request;

    let z = x / y;
    ensure z != 0; // for some reason. just as an example
    return z;
}
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


fn string x_to_string(X x) => switch x {
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
struct Vector3 {
    f32 x, y, z;
};

// Unlike in C, you don't need to typedef structs to avoid the struct keyword in declarations
// Also unlike in C, structs cannot be anonymous
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

// NOTE: Do structs really need methods?
// what if instead of "true" methods there was Uniform function call syntax instead?

int add(int x, int y) {
    return x + y;
}

let x = 10;
let y = 30;
let z = x.add(y); // => 40
let w = z.add(y).add(x); // => 80. equivalent to add(z, add(y, x));

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

# Arrays 
```c

```

# Enums 
```c 

// just like C's enums, except with their own namespace

enum Colour {
    Red, Blue, Green, White, Yellow, Brown
};

enum MnMs {
    Red, Blue, Green, Yellow, Brown, Orange
};

let x = Red; // => Error: Assigning variable <x> to nknown value <Red>;
let x = Colour.Red; // => compiles
let y = MnMs.Red; // => Also compiles


```



# Custom Types

```c

// You can define your own types by aliasing other types
alias u32 = int;

u32 x = 10; // base type: int

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
    io:println("Hello!");
}

import math as m;

m:abs();


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
interface debug => {
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
// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
int[4] numbers = { 10, 20, 30, 40 };
// first, arrays have their length encoded into them
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size 4 with have four elements in it but the final element will have 
// the index [3], as indices begin at [0]
numbers.length // => 4
// second, you can initialize arrays with default values
int[0;10] numbers; // => array of size 10 with every element being initialized to 0
int[5;8] numbers; // => array of size 8 with every element being initialized to 5

// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
int print_num_array ( int[10] numbers ) { /*...*/ }

int *nums;
print_num_array(nums); // ERROR: Incompatible types; expected type u32[10], got u32* instead;

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
*ptr // => dereference pointer, 10
&ptr // => address of val

```

# Misc + Reference

## Character / operator list:
\+ \- \* / % ! != += -= *= /= %= ^ ~ & | && ||
. , > >= <= < ? : ; ' " = == ( ) { } [ ]
@ .. ... -> =>

## Keyword list 

keywords from C:

short char int long float double void
if else for while break continue
goto struct return 
enum union sizeof const

river keywords:

usize isize let str bool array byte
foreach defer match label
public private sticky
new delete namespace macro
impl interface import as from with is mut


## Notes
you can ignore this bit

how about smarter comments?
tagged comments
like @todo @error @bug @note, which the compiler could build notes out of and then store in a file somewhere?

maybe add language packs as a build flag, or a file annotation?
like

what if you could define your own keywords?


```c 
@lang: 'ur'

اگر (x = y) {
    لکہو("سلام")؛
}
// struct
شکل {
حندسہ x, r
}
```

# Grammar specification

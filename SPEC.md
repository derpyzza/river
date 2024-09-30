# Introduction
This is the language spec for river.

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
 *  numbers     - signed, unsigned, integer, floating point. 
 *  string      - "string"
 *  true
 *  false
 */

```



# Variable Definition & Declaration

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

// You can assign multiple variables at once:
u32 x = 20, y = 50, z = 40;
// Even if the types are separate:
let x = "X", y = 230, z = false; // perfectly valid

// Multiple variables can be initialized with the same value, but their types must be the same
u32 x, y, z, w = 1;

// variables are non-nullable by default
// to have a nullable variable, it must be explicitly marked as such
// by appending a '?' to the variable name
// Unlike pointers, nullability is a datatype
str? name = get_name();

// nullable variables must be checked before use 
printf(name); // ERROR: Value is nullable
if name {
    printf(name); // compiles
}


```

# Functions 

```c

// functions are defined using the following syntax:
u32 add ( u32 x, u32 y ) {
    return ( x + y );
}

// functions are called as such:
add(10, 20); // => 30

// Code blocks, including functions, support implicit returns
// the last expression within a block is returned
u32 sub ( u32 x, u32 y ) {
    x + y // returns x + y
} 

struct Colour(u32: r, g, b, a);

// Functions support default parameter values
// If an argument is not passed for a particular parameter,
// the default value is used instead.
Colour Colour_from_rgba ( u32 r, u32 g, u32 b, u32 a = 255 ) {
    return Colour(r, g, b, a);
}

// Arguments with default parameters can be omitted
Colour_from_rgba(125, 125, 125); // param 'a' => 255;
Colour_from_rgba(125, 125, 125, 125); // param 'a' => 125;

// Function calls also look similar to C
Colour_from_rgba(125, 125, 125, 255);

// Function arguments can be named when calling functions
// Take the following function for example:
Rectangle draw_rect_pro (u32 x, u32 y, u32 width, u32 height, f64 rotation, Colour col);
// You can call the function by naming the individual arguments
Rectangle rect = draw_rect_pro(x: 20, y: 30, width: 240, height: 360, col: (10,10,10,255));
// The advantage of named arguments is that you can rearrange the order in which you pass them:
draw_rect_pro(width: 240, height: 360, x: 20, y: 30, col: (10,10,10,255)); // perfectly valid

// functions can have multiple return values by utilizing tuples
(File, Error) read_file (str path) {
    // ... file reading logic ...

    return (file, err);
}

let (file, err) = read_file("SPEC.md");
if err {
    error(err);
}

// In the case of functions that return nullable values, you can unwrap
// the value using the '?' operator:
str name = get_name()?; // => if get_name() returns null then the program panics;

// functions can be declared using this alternate syntax:
u32 add => (u32 x, u32 y) { ... }

// NOTE: are closures necessary?
// This syntax is handy for closures
let x => () {
    30
};
x(); // => 30

// Closures can have function parameters:
let x => (u32 x) {
    10 * x
}; 
x(10); // => 100

// functions are first class in river, so you can pass them around as values
// TODO: workshop the function callback syntax a bit more
u32 example_func(f32 val, func => <void(u32, u32)> ) 
{
    func(20, 20); // => calls the passed-in function
}

// what should the closure function signature syntax be?
u32 example_func(f32 val, func => void(u32, u32)); 

u32 example_func(f32 val, func => <void(u32, u32)>); 

u32 example_func(f32 val, func => void|u32, u32|);

u32 example_func(f32 val, void func => (u32, u32));

u32 example_func(f32 val, void func(u32, u32));

// functions can return other functions
bool(u32) func () {
    // functions can be defined inline as closures
    return bool(u32 x) => {
        if x {
            true
        } else {
            false
        }
    };
}

```

# Control flow

```rs

// all the following control flow structures are expressions rather than statements

let x = if bool { 10 } else if other_bool { 11 } else { 23 };
let x = if (bool) 10 else if (other_bool) 11 else 23;

if x == "string" {
    do_something();
}
if ( x == "string" ) {
    do_something();
}


if ( expression ) {
    // do stuff
} else if ( other_expression ) {
    // do other stuff
} else {
    // do yet more stuff
}

if expression { statement };

while ( expression ) {
    // do stuff
}

do {

} while expression;

for (init; condition; step) {

}

for (x = 0; x < 10; x++) {

}

foreach x in range(0..=10) {

}

foreach item in iterable {

}

match expr {
    case => expr,
    case => {...},
    case,
    case,
    case => {...},
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

struct Cat {
    Vector3_s pos, orientation;
    str name;
    str mew_style;

    // Structs can contain methods within them
    void Meow(Self) {
        printf("%s", Self.mew_style);
    }
};

Cat kitty = new Cat;
kitty.mew_style = "mewmewmew";
kitty.Meow(); // mewmewmew

// If you have a struct where all it's members are the same type, you can define it 
// using the following bit of syntactic sugar:
struct Colour(u32: r, g, b, a);
// With the syntax pattern being:
struct name(type: val1, val2, val3 ...);

```

# Tuples
```c 

// tuples are like structs, except they don't have named parameters

(u32, string) x = (23, "hello");

let (id, name) = (2332, "derpy");
let tup = (23, "dwe", 36);

```



# Custom Types

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

# Namespaces
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
interface cat<T> => where T is Stringy {}

int count_entities(T entities) => where T impls Entity {
    for t in entities {
        t.print_name();
    }
}

```

# Arrays & Slices

```c

// river's arrays are almost the same as C's, with some key differences
// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
u32[4] numbers = { 10, 20, 30, 40 };
// first, arrays have their length encoded into them
// Do note: the array length is equal to the number of elements in it, not the max index
// i.e an array of of size 4 with have four elements in it but the final element will have 
// the index [3], as indices begin at [0]
numbers.length // => 4
// second, you can initialize arrays with default values
u32[0;10] numbers; // => array of size 10 with every element being initialized to 0
u32[5;8] numbers; // => array of size 8 with every element being initialized to 5

// Unlike in C, river's arrays are a distinct type and cannot dissolve down to pointers
u32 print_num_array ( u32[10] numbers ) { ... }

u32 *nums;
print_num_array(nums); // ERROR: Incompatible types; expected type u32[10], got u32* instead;


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


// Statements

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
. , > >= <= < ? : ; ' " """ ` = == ( ) { } [ ]
@ .. ...

## Keyword list 

keywords from C:

short char int long float double void
if else for while break continue
switch case goto struct return 
enum union sizeof const

river keywords:

usize isize let str bool array byte
foreach defer match label
public private sticky
new delete namespace macro
impl interface import as from with is


## Notes
you can ignore this bit

how about smarter comments?
tagged comments
like @todo @error @bug @note, which the compiler could build notes out of and then store in a file somewhere?

maybe add language packs as a build flag, or a file annotation?
like

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

design doodles

river design goals / considerations
- fixes some of C's annoyances
- Good, seamless interop with C ( or near-seamless )
    C can call river code, river can call C code
    you can mix and match C and river code in the same codebase as you wish
- Emphasis on testing and code correctness
    the language and it's toolkit should come with built-in support for unit testing
    the compiler should help check that the behaviour is correct
- Emphasis on standards and documentation
    code should be easy to document
    compiler or it's tooling should be good at generating documentation
- Orthogonal but handy featureset
  - features should be orthogonal, no useless bikeshedding for the sake of it ( with some minor exceptions here and there ;) )
- unload as much work on to the compiler as possible
    - the compiler is generally better at doing tedious stuff than humans are, so unload all of that onto the compiler
    - Having fancy cool metaprogramming features and compile time code execution also comes into this. there are often times where you need to write code that you could also just programatically generate, and i want to make it easy to generate all that code in river.
    - i like jai's strategy of having compile-time code be the same as runtime code except marked with a keyword. really cool. do that. maybe.
- better type system
    C's typesystem is basically a very weak representation of types
    types are essentially a bag of bytes, and you can reinterpret types as each other willy nilly
    river's type system should aim to be a bit stronger and a bit more expressive, and yet provide constructs to be able to twiddle with the individual bits if needed
- safety + control
    the compiler should do it's best to check your work and make sure that it's safe / correct, by default
    however, it should also acknowledge that you, the programmer, know what you're doing and should hand over control to you when you ask for it
    in other words, footguns are off by default, but you can explicitly enable them and shoot yourself in the foot if you truly desire to
- explicit is better than implicit
    the compiler shouldn't randomly engage in hidden behaviours
    if you want to do something, it should be explicit
- modular compilation units
    river source code should compile down into separate "implementation" and "interface" units, with the interface unit being like a fancy hashmap that the compiler can easily traverse and only extract the data that it needs.
    the final code then compiles down into one single C file with all the declarations at the top and the definitions at the bottom
- GOOD BUILD SYSTEM
  - the compiler should be able to handle cross-compilation, debug vs release builds and all that jazz with minimal nonsense.

DRY, i shouldn't have to declare the same thing in multiple spots. No function prototype at the top of the file nonsense, declare functions where ever. However, do declare variables before you use em.

i want reflection

stylistic rules:
- built-in types are lowercase
- user-defined types are CamelCase ( including stdlib types )
- function names are pascalCase ( including built-in functions )
- variables are pascalCase
- struct fields are pascalCase

feature list:
- type inference
- default values
- algebraic data types
- method call syntax
- distinct and aliased type defs
- maybe struct destructors / some method of resource management tbh
- arrays with size included ( same with strings ) | runtime bounds checking? 
- slices ( Fat pointers )
- visibility modifiers
- ad-hoc unions?
- struct methods
- array ranges
- defers
- tuples ( basically structs with anonymous position-based fields )

- operator overloading ( somehow ) | or should i ???
- GENERICS??
- INTERFACES????
- REFLECTION ????
- COMPILE-TIME CODE EXECUTION???

prolly replace alias with const

what if attaching a $ to something made it run in compile time?


i want some kinda opt-in structural typing. the applications for this would be *pretty* niche, and frankly it might get a little annoying? but i'd still like an option for it for when
there are multiple different types of the same exact data in the same scope and you need to use em interchangably:

// libs/nuklear.rvr

type nkcolour = struct {
  r, g, b, a: f32;
}

// gfx.rvr

import libs:nuklear as nuk;

// a shape type is like an interface that defines what a data structure should *look* like
type Colour = shape {
  r, g, b, a: f32;
}

nuk:nkcolour implements Colour;

fun do_smth(c: Colour) { ... }

let red = nuk:nkcolour(255, 0, 0, 1)
do_smth(red) // works



printf("runtime");
$printf("comptime");

$repeat {
  // this entire block is compile time
} until true;

type Smth = struct($T, $U) {
  foo: T,
  bar: U
};

i should prolly allow methods back in....

I WANT LAMBDAS

let x = fun(i) => i * i;
x(5); // => 25


switch value as v { ... };
import x as y;


odin/jai style using statement. brings struct fields into scope. can be used for subtype polymorphism:
type Entity = struct {
  x, y, z: int;
}

type Enemy = struct {
  health: int;
  using entity: Entity;
}

fun Move_To(entity: Entity) {
  // whatever
}

let zombie = Enemy.{};
Move_to(zombie); // valid

else, else return and else continue operators like odin's or_else, or_return and or_continue operators?


packed enums / bit fields
type WindowFlags = packed enum {
  Fullscreen,
  Resized,
  Blah,
};
=> packs into number.
let x: WindowFlags = WindowFlags.Fullscreen and WindowFlags.Resized; // 0x0011 basically??
if(x.Fullscreen) // automatically extracts it via the bitmasking stuff


overloaded operators should be marked as such?

Vec1 \+ Vec2
// smth like that?



struct inheritance?

type Entity = struct {
  pos: Vec2;
  health: int = 100;
}

type Enemy = struct : Entity {
  attack: int; 
}

fun spawn_enemy(e: *Enemy) {
  e.pos = Vec2_random();
}

let goomba: *Enemy = new(Enemy);
goomba.health = 150;
spawn_enemy(goomba); // acceptable;

type GameObject = interface {
  pos: Transform,

  fun spawn(self: Self);
  fun moveTo(self: Self, to: Tile);
}

type troop = struct : GameObject {
  
}


type Rectangle = struct(width: int = 1, height: int = 1) {
  perimeter: int = 2 * (height + width)
  area: int = height * width
}

let rect = Rectangle(5, 5);

variable shadowing should be allowed.

# MEMORY MANAGEMENT

boxed pointers?
- like some sort of differenciator between "owning" pointers and "viewing" pointers?

maybe like, pointers vs references?
pointers own an object and are responsible for calling free somewhere before the scope ends
but references are not allowed to call free at all?

maybe this should all be enforced in the stdlib and not in a language level?

what if destructors were allowed in the language, but they were opt-in and not automatic?

```c++

// manually managed.
let object = Object::new();
defer object.delete();

// this one is manually marked as an acquired resource, which means that it automatically calls a destructor??
let object2 = acquire Object::new();
  
```

though obviously this does bring up questions like, how are values allocated?
can every object declare it's own `new` function or must they all follow the same pattern?
what if an object doesn't have a destructor? is there a default destructor?
is there a default constructor too?

what if instead of using a `new` function that's defined on an object, it instead used a built-in `new` function that allocates a new object?

```c++

let object := new(Object); // allocates 1 object and returns a slice to that memory.

let objects := new(Object, 10); // allocates 10 objects at once and returns a slice to that memory.

let debugObjects := new(Object, 10, allocator: DebugAllocator); // allocates 10 objects and uses a custom allocator.

// similarly there should prolly be a delete function:
// it just frees memory, that's all

delete(object);
delete(objects);
delete(debugObjects);
```
```rs

fun new(T: type, num: size, alloc: Allocator): *type;

fun new<T>(t: T, num: size, alloc: Allocator): *T;

let object = new<Object>::();

let object = new(Object);

let object = new(Object);
let objects = new(Object, 10);
  => {
    let data: [*]T = alloc.alloc(Object, 10);
    for d in data {
      d.init();
    }
  }
  
```

what if the language had constructors and destructors, and the new/delete functions implicitly call the constructor/destructor for an object after allocating memory for it?
the constructors could be used for ensuring that the memory/object is formatted properly before use / is in an expected state.
and then the destructors could be used for ensuring that the cleanup is done correctly?



explicit fallthrough in switch statements

version management?

despite having a module system it shouldn't be *necessary*.
i.e you should just be able to a single file library without needing to
mess around with a module system.
like
json.rvr <- full library.
like single header file libraries in C
and you should be able to slap multiple libraries in the same directory and have them work
like
libs/
|-json.rvr
|-glm.rvr
|-stb_image.rvr

and then in main.rvr
import libs:json;
import libs:glm;
import libs:stb_image;


error on unused variables, but only in release mode.


if structs have no associated functions, and i implement the ufcs thingy, then technically
functions are in the global scope and not in module scope??
see:
a.b() == b(a)
was my original premise. but consider that modules foo and bar both have a function
b that takes in a variable of type a
foo::b(a), bar::b(a)

how do i differenciate between them when doing the ufcs thingy?
a.foo::b(), a.bar::b()?
foo::a.b()?

what if i had an "applicator" operator?
the intention with the ufcs thingy is to be able to do this

a.b().c().d() == d(c(b(a))).

what if i had an operator that let me apply these functions in the way i wanted to?

a -> b() -> c() -> d();
a -> foo::b() -> bar::c() -> baz::d();

a little verbose but it resolves the issue?

let v = Vec3();
v
  -> Vec3:translate({19, 3, 2})
  -> Vec3:rotate({1, 0, 1}, 20)
  -> vec3:scale({0, 1, 0}, 10);
this would be equivalent to doing
  vec3:scale(
    Vec3:rotate(
      Vec3:translate(v, {19, 3, 2}), {1, 0, 1}, 20),{0, 1, 0}, 10);

or, in a less stupid way:

v = vec3:scale(v, {0, 1, 0}, 10);
v = Vec3:rotate(v, {1, 0, 1}, 20)
v = Vec3:translate(v, {19, 3, 2})

[10, 20, 35, 35, 64, 57]
  -> filter( (x) => x >= 35 )
  -> map( (x) => x - 10 )
  -> collect<[[*]int]>();



glm_mat4_add(glm_mat4_mul(x, y), z);
vs
x.mul(y).add(z);
vs
x * y + z;

access modifiers?
// visibility modifiers
- private // default, implicit, private to module
- local   // private to file
- public  // visible to other modules
- readonly // visible to other modules, but only writeable by current module
- internal readonly // visible to other modules, but only writeable by current file
// mutability modifiers
- mutable
- immutable // default

a note on readonly:
  readonly signifies that a variable is public, but can only be edited by the struct that it's a part of.


public let x = 10;
readonly let x = 25;

let public x = 10;
let readonly x = 2424;

let x: public int = 10;
let x: readonly int = 10;

_idens are private
non_underscore are public
!idens are readonly?

for $field in $fieldsOf(struct) do ...
vs
for $field in @fieldsOf(struct) do ...
vs
for $field in #fieldsOf(struct) do ...

let x: int = 10;
let y: ^int = &x;
let z: int = @y;


#fieldsOf
#declsOf
#ifdef
#ifndef
#deprecated

// struct
// destructor function, runs at the end of the scope?
fun destroy() {
    
}


# BUILTINS
#include // compile time textual include like in C
import @C("")
sizeof typeof alignof


## Operator overloading

limits on operator overloading:
- Must not allocate
- Must not error
- Must not return null
- Must follow a very specific function signature

type Vec2 = struct {
    x, y: f32;

    #op(+)
    fun add (a, b: Vec2) -> Vec2 = Vec2 {a.x + b.x, a.y + b.y};
    #op(-)
    fun sub (a, b: Vec2) -> Vec2 = Vec2 {a.x - b.x, a.y - b.y};
    #op(*)
    fun mul (a, b: Vec2) -> Vec2 = Vec2 {a.x * b.x, a.y * b.y};
    #op(/)
    fun div (a, b: Vec2) -> Vec2 = Vec2 {a.x / b.x, a.y / b.y};
    #op(-, prefix)
    fun neg (v: Vec2) -> Vec2 = Vec2 {-v.x, -v.y};
    #op(==)
    fun eq (a, b: Vec2) -> bool = (a.x == b.x) && (a.y == b.y);
    #op(!=)
    fun neq (a, b: Vec2) -> bool = (a.x != b.x) || (a.y != b.y);
};

// other operations include:
// gt        : greater than `>`
// lt        : less than `<`
// gteq      : greater than or equal to `>=`
// lteq      : less than or equal to `<=`
// get_item  : for loop iterations `in`
// get_index : subscript set `[]`
// set_index : subscript get `[]`  
// add_assign: +=
// sub_assign: -=
// mul_assign: *=
// div_assign: /=
// mod_assign: %=

// function signatures
op eq (a, b: T) -> bool;
op neq (a, b: T) -> bool;
op >= gteq (a, b: T) -> bool;
op <= lteq (a, b: T) -> bool;
op < lt (a, b: T) -> bool;
op > gt (a, b: T) -> bool;
op + add (a, b: T) -> T;
op - sub (a, b: T) -> T;
op * mul (a, b: T) -> T;
op / div (a, b: T) -> T;
op - neg (v: T) -> T;
op [] get_item (a: []T) -> T;
op [] get_index (a: []T) -> T;
op [] set_index (a: []T) -> T;
op += add_assign(a: ^T, b: T);
op -= sub_assign(a: ^T, b: T);
op *= mul_assign(a: ^T, b: T);
op /= div_assign(a: ^T, b: T);

fun main() {

  let x = (x) => x * x;

  let d = file.read("foo.txt");

  d = d
    -> foreach()
    -> map(x)
    -> collect();
  
}


// gfx/base.rvr
module base;

pub type VAO = u8;
pub type EBO = u8;
pub type VBO = u8;

pub
local

// mesh.rvr
module mesh;
import gfx::base;

pub type Mesh = struct {
  vao: base.VAO,
  ebo: base.EBO,
  vbos: [*]base.VBO,

  v_pos:  [*]f32,
  v_col:  ?[*]f32,
  v_tex:  ?[*]f32,
  v_norm: ?[*]f32,
  v_tex1: ?[*]f32,
};



// std/string.rvr
module string;

pub type Rune = u32;
pub type String = struct {
  data: [*] Rune;
  cur: usize;
};

pub fun from(str: string): String;
pub fun split(str: String): String;
pub fun trimr(str: String, c: char, num: int = 1): String;
pub fun triml(str: String, c: char, num: int = 1): String;
pub fun substr(str: String, on: char = ' '): [*]String;

// main.rvr
import std:string;

fun main() {
  let s = string:from("##hello,there!$$");
  s -> string::trimr('#', 2)
    -> string::triml('$', 2)
    -> string::substr(',')[0];

  io:print("s: \"{}\"", s); // s: "hello"
}


// main.rvr

import parser, scanner, codegen;

// each of these modules are stores in .rvr files.
// however if i were to split parser.rvr into multiple files in a directory:

parser/
  | ast.rvr
  | mod.rvr
  | expr.rvr
  | types.rvr
  | debug.rvr

i should still be able to just use `import parser;` and have it resolve to the proper module

i could do reexports?
parser.rvr could be edited to be like:
```
using ast;
using expr;
using types;
using debug;
```

and then other files can still just refer to parser as a single module??
though this does mean that you'd then just have a directory that looks like:

 | main.rvr
 | scanner.rvr
 | parser.rvr
 | parser/
 | | expr.rvr
 | | types.rvr
 | | debug.rvr
 | | ast.rvr

which is kinda icky

maybe imports should search for modules in a set pattern?
like first look for `.rvr` files with that name, and if that doesn't exist then for directories with that name?
if neither exists then that's a [MODULE_NOT_FOUND] error and if both exist then that's a [MODULE_DUPLICATED] error and it should be resolved.


```
type Ast = union {

  Expr: union {
    BinaryExpr: struct {
      lhs: *Expr;
      rhs: *Expr;
      op: *Token;
    } "binary expression",
    UnaryExpr: struct {
      op: *Token;
      val: *Expr;      
    } "unary expression"
  } "expression",

  If: struct {
    cond: *Expr,
    
  }
}
  
```


fun colourFunction (data: struct{x,y,z,w: f32}) {...}

type Colour = struct {r, g, b, a: f32}
type OtherColour = struct {r, g, b, a: f32}

let x = Colour(1, 1, 1, 1);
let y = OtherColour(0, 1, 0, 1);

colourFunction(x); // OK
colourFunction(y); // Also OK

[x, y, z] => colourFunction();
// equivalent to:
colourFunction(x);
colourFunction(y);
colourFunction(z);

fun doSomething(str: string, flag: bool) {}

[
  ("string", true),
  ("hello", false),
  ("name", true),
] => doSomething; // calls doSomething with each of the values given in that array:

doSomething("string", true);
doSomething("hello", false);
doSomething("name", true);


[
  ("string", true),
  ("hello", false),
  ("name", true),
].forEach( fun(x: string, y: bool) = doSomething(x, y) );

let vars = [
  ("string", true),
  ("hello", false),
  ("name", true),
];

for v in vars {
  doSomething(v.0, v.1);
}
  
using std.io.file;

fun main() {
  let f = file.open("input.txt")?;
  let data = f.read();

  data
    .split('\n')
    .forEach((s) => s.split(' '))
    .filter((s) => s.startsWith('#'))
    ;
}

type Message = union {
  Quit,
  Move: { x: i32, y: i32 },
  Write: String,
  ChangeColor: (i32, i32, i32),
}

```go
type Point3D = {
  x, y, z: f32;
} // struct type

type RGB = (f32, f32, f32); // tuple type

type Number = int | f64;

type Ast =
  | BinExpr: {lhs: *Ast; op: char; rhs: *Ast}
  | UnExpr: {op: char, val: *Ast}
  | Identifier: string
  | Number: int | f64
  ;
```

ArrayList<int>::new() vs ArrayList::new(int);

Foo<Type, Value>::new() vs Foo(Type, Value);

HashMap<string, int>::new() vs HashMap(string, int);

let x: HashMap(string, int) = HashMap(string, int, 10);

fun HashMap( type: (key: $T, value: $V) ): HashMap(T, V);

fun something( t: typeid ): *typeid {
  
}
  


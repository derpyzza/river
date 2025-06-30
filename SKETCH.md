design doodles

river design goals / considerations
- smaller than C++
- bigger than C
- fixes some of C's annoyances

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


variable shadowing should be allowed.

boxed pointers?
- like some sort of differenciator between "owning" pointers and "viewing" pointers?

maybe like, pointers vs references?
pointers own an object and are responsible for calling free somewhere before the scope ends
but references are not allowed to call free at all?

maybe this should all be enforced in the stdlib and not in a language level?


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


```  
! logical not?
@ builtin functions? dereference operator?
# compiler directives?
$ comptime variable and stuff?
% modulus
^ xor
& and
* multiplication, pointer
- minus
= assignment
+
[ ]
( )
{ }
\
|
'
"
:
;
,
.
<
>
/
?
~
.* pointer dereference
```


## Operator overloading

limits on operator overloading:
- Must not allocate
- Must not error
- Must not return null
- Must follow a very specific function signature

```c

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
    
```


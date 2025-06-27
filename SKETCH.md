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

glm_mat4_add(glm_mat4_mul(x, y), z);

vs

x.mul(y).add(z);

vs

x * y + z;



let x = 0b1110 and 0b0011 xor ((neg 0b1001) or 0b1111);
let x = 0b1110 & 0b0011 ^^ ((~0b1001) | 0b1111);
let x = 0b1110 && 0b0011 ^^ ((~0b1001) || 0b1111);



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


access modifiers?
- public
- private // default, implicit
- readonly
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

@public
let x = 22;
@readonly
let x = 22;


public {
  let x = 22;
  let x = 22;
  let x = 22;
}

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


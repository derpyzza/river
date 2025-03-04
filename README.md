# river
> [!CAUTION]
> This project is currently under construction, so most of what you see here is:
> 1. very, very broken right now, and
> 2. subject to change.
> ---
> With that in mind however, feel free to take a look around, and drop some feedback if you'd like. 
> However, do note that since this project is currently in it's infancy, i'm not accepting any proper contributions for now.

A Small C-inspired language that's transpiled down to human-readable C99 code.

named river 
cause it flows down to C ( badum tiss )

River is a programming language for those who like the DIY-nature of C, but sometimes wished that they had access to the convenient higher level features from other languages ( but are too scared of C++ ).
Simply put, river is a dialect of C with a boatload of syntax sugar and niceties piled on top, along with a side of an expressive type system and garnished with some really nice functional features to top it all off.

The goal is to be able to seamlessly use both C and river in the same codebase, and replace C files with river as needed ( and river files with C, if you want to ).

Outputs to a human readable C99.

River offers the following feature-set over plain-C:
- An easier to parse syntax ( for machines and humans alike )
- Type inferencing and a stronger type system
- Arbitrary compile-time code execution / stronger macros
- Expression-oriented syntax
- Default function arguments / struct fields
- Function overloading ( functions are identified by their signatures, not just their names )
- Limited operator overloading ( limited to basic arithmetic, comparisons and subscripting )
- Algebraic data types ( discriminated unions, mostly )
- Method call syntax
- Nicer array and string types
- Pattern matching
- Localized Namespaces ( that is, there are more namespaces than just "global" and "local" )
- Interfaces
- Generics
- Distinct and Aliased type definitions ( or nominal vs structural types )
- A proper module system

And gets rid of:
- The pre-processor
- Pointers decaying into arrays
- Implicit type conversions
- Prefix decrement/increment operators ( ++ / -- )
- Implicitly nullable pointers

# Quick start
This repository only contains the river compiler for now.
The river compiler doesn't depend on any external dependancies directly, though you will need
a C compiler ( river uses `clang` by default ) and some version of `make` in order to build the compiler. Once you've acquired these,
just simply run:
```sh
$ make
```
and it will compile the river compiler `rcc` to the `bin/` directory.

If you wish to use a C compiler other than the default `clang`, you can do so by using the following command instead:
```sh
$ make CC=$(YOUR_C_COMPILER)
```

From there you can use the river compiler to compile river programs by simply running:
```sh
$ rcc in.rvr
```

# Language Features

## Automatic type inference

```rs 

// This
let x = 23; // implicitly int
let y = 20.0; // or 20.0, 20f; all = 20.0

// Equals
let x: int = 23;
let y: f32 = 23.0;

```

## Default values

```go 

type object = struct {
    param1: int;
    param2: int = 30;
};


fun function_definition ( x, y: int = 20, has_something: bool = true ) = {
    // define function over here
};

```

## Tuples

```c rvr
let x = (23, 59, "String");

// a gets set to x.0, b gets set to x.1
let a, b = x;

// the parenthesis in a tuple are optional:
let a, b, c = 1, 2, 3;
// this destructures a tuple with elements ( 1, 2, 3 ) into three separate variables.
// similarly, you can bind a tuple to a single variable and access the tuples elements individually:
let x = 1, 2, 3;

// in the case of variable destructuring, the number of variables on the lhs must match those on the rhs:
let x, y = 1, 2, 3; // => error, too many values
let x, y = 1, 2; // fine
```

## Algebraic Data Types

```rs

type Value = enum {
    Int: int,
    Float: float,
    String: string,
    Ptr: ^Value  
};

fun print_value(v: Value) -> string {
    let s: string;
    match v {
        Int => s.format("int: {}", v),
        Float => s.format("float: {}", v),
        String => s.format("string: {}", v),
        Ptr => s.format("ptr to: {}", print_value(v)),
    }
    
    return s;
}

fun add_value(v, y: Value) -> ?Value = if (v int and y is int) Some(v + y) else None;
```

## Defers
The `defer` keyword just executes the given statement at the end of the current scope.

```c 

struct object {...};

fun main() {
    let x = new_object();
    defer delete_object;

    /*
     * do stuff ...
     */

    // calls delete here
}

```

## Pattern matching & Destructuring
```c 

switch user.rank {
    "Employee" => {},
    "Admin" => {},
    _ => {}
}

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
};


op add (a, b: Vec2) -> Vec2 = Vec2.{a.x + b.x, a.y + b.y};
op sub (a, b: Vec2) -> Vec2 = Vec2.{a.x - b.x, a.y - b.y};
op mul (a, b: Vec2) -> Vec2 = Vec2.{a.x * b.x, a.y * b.y};
op div (a, b: Vec2) -> Vec2 = Vec2.{a.x / b.x, a.y / b.y};
op neg (v: Vec2) -> Vec2 = Vec2.{-v.x, -v.y};
op eq (a, b: Vec2) -> bool = a.x == b.x && a.y == b.y;
op neq (a, b: Vec2) -> bool = a.x != b.x || a.y != b.y;

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
op gteq (a, b: T) -> bool;
op lteq (a, b: T) -> bool;
op lt (a, b: T) -> bool;
op gt (a, b: T) -> bool;
op eq (a, b: T) -> bool;
op add (a, b: T) -> T;
op sub (a, b: T) -> T;
op mul (a, b: T) -> T;
op div (a, b: T) -> T;
op neg (v: T) -> T;
op get_item (a: []T) -> T;
op get_index (a: []T) -> T;
op set_index (a: []T) -> T;
op add_assign(a: ^T, b: T);
op sub_assign(a: ^T, b: T);
op mul_assign(a: ^T, b: T);
op div_assign(a: ^T, b: T);
    
```
## Interfaces

## Generics

```c

fun add[T: type where T is Addable](x, y: T) -> T = x + y;

    
```

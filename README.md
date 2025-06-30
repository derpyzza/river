# river
> [!CAUTION]
> This project is currently under construction, so most of what you see here is:
> 1. very, very broken right now, and
> 2. subject to change.
> 3. probably already outdated, as the language and it's current implementation change faster and more often than the documentation for it.
> ---
> With that in mind however, feel free to take a look around, and drop some feedback if you'd like. 
> However, do note that since this project is currently in it's infancy, i'm not accepting any proper contributions for now.

A Small C-inspired language that's transpiled down to human-readable C99 code.

named river 
cause it flows down to C ( badum tiss )

River is a lightweight programming language for those who like the DIY, hands off nature of C, but wish for a more modern version with convenience features on top.

The goal is to be able to seamlessly use both C and river in the same codebase, and replace C files with river as needed ( and river files with C, if you want to ).

Outputs to a human readable C99.

River offers the following feature-set over plain-C:
- An easier to parse syntax ( for machines and humans alike )
- Type inferencing and a stronger type system
- Default function arguments / struct fields
- Function overloading ( functions are identified by their signatures, not just their names )
- Limited operator overloading ( limited to basic arithmetic, comparisons and subscripting )
- Algebraic data types ( discriminated unions, mostly )
- Method call syntax
- Distinct and Aliased type definitions ( or nominal vs structural types )
- Defers
- A proper module system
- Nicer array and string types
- Arbitrary compile-time code execution / stronger macros
- Expression-oriented syntax
- Pattern matching
- Interfaces
- Generics

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

## Method call syntax
```
fun Foo(x: Bar) {...}

let bar: Bar = Bar();
bar.Foo(); // Same as calling Foo(bar);

A( B( C( D ))) === D.C().B().A()
    
```

## Array shorthands
```

type name = [4]char;

let arr: [256]name = [
    "BEST", "DEST", // regular array items, at indexes 0 and 1;
    34 = "DIRT", // set item at index 34 to "DIRT"
    52..64 = "BERT", // set items from index 52 to index 64 to "BERT"
    ... = "NONE" // set every item in the array (besides the ones we set manually) to "NONE"
    ];
    
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

## Defers
The `defer` keyword just executes the given statement at the end of the current scope.

```c 

struct object {...};

fun main() {
    let x = new_object();
    defer delete_object();

    /*
     * do stuff ...
     */

    // calls delete here
    // delete_object()
}

```

## Tuples

```c rvr
let x = (23, 59, "String");

// a gets set to x.0, b gets set to x.1
let (a, b) = x;
let x = (1, 2, 3);

// in the case of variable destructuring, the number of variables on the lhs must match those on the rhs:
let (x, y) = 1, 2, 3; // => error, too many values
let (x, y) = 1, 2; // fine
```

## Algebraic Data Types + Pattern matching

```rs

type Value = enum {
    Int: int,
    Float: float,
    String: string,
    Ptr: *Value  
};

fun print_value(v: Value) -> string {
    let s: string;
    switch v {
        Int(v)    => s.format("int: {}", v),
        Float(v)  => s.format("float: {}", v),
        String(v) => s.format("string: {}", v),
        Ptr(v)    => s.format("ptr to:\n\t{}", print_value(v.*)),
    }
    return s;
}

fun add_value(v, y: Value) -> Optional<[Value]> =
    Some(v + y) when Value::Int(v) and Value::Int(v)
    else None;
```

## Generics

```c

fun add<[T]>(x, y: T) -> T = x + y;

add<[f32]>(0.2, 0.4); // => 0.6
add<[int]>(3, 5) // => 8


fun sort<[T: Comparable]> ( x: Array<(T)> ) {
    //...//
}

sort[int]( Array[int] { 35, 13, 1, 545 } ); // => Array[int]{ 1, 13, 35, 545 }

type num = struct {
    x: int;
};

sort[num]( Array[num]{ ... } ); // ERROR, type <num> does not implement interface Comparable

    
```

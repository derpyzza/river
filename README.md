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
- Generics
- Namespaces
- Visibility control ( defaults to private )
- Mutability control ( defaults to immutable )
- Default function arguments / struct fields
- Function overloading
- Distinct and Aliased type definitions
- Multiple function return values
- Method call syntax
- Type inferencing and a stronger type system
- Algebraic data types
- Pattern matching
- A proper module system
- Expression-oriented syntax
- First class testing framework
- Nicer array types
- An easier to parse syntax ( for machines and humans alike )

And gets rid of:
- The pre-processor
- Pointers decaying into arrays
- Implicit type conversions
- Decrement/Increment operators ( ++ / -- )
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

# Language Goals and Motivations
# Ecosystem Features
## Testing Framework
## Documentation Generation
## Package management

# Language Features

## Automatic type inference

```c 

// This
let x = 23; // implicitly int
let y = 20.; // or 20.0, 20f; all = 20.0

// equals:
int x = 23;
double y = 23.0d; // floating point numbers evaluate to double by default


```

## Tuples

```c rvr
let x = (23, 59, "String");

// a gets set to x.0, b gets set to x.1
let a, b = x;
```
## String Formatting
## Generics
## Algebraic Data Types
## Comptime code execution 

## Defers
The `defer` keyword just executes the given statement at the end of the current scope.

```c 

struct object {...};

int main() => {
    let x = new_object();
    defer delete_object;

    /*
     * do stuff ...
     */

    // calls delete here
}

```

## Default values

```c 

struct object {
    param1: int,
    param2: int = 30,
}

fun function_definition ( x: int, y: int = 20, has_something: bool = true ) = {
    // define function over here
};

```

## Pattern matching & Destructuring
```c 

switch user.rank {
    "Employee" => {},
    "Admin" => {},
    _ => {}
};

```

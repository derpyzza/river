# river
> [!CAUTION]
> This project is currently under construction, so most of what you see here is:
> 1. very, very broken right now, and
> 2. subject to change.
> ---
> With that in mind however, feel free to take a look around, and drop some feedback if you'd like. 
> However, do note that since this project is currently in it's infancy, i'm not accepting any proper contributions for now.

A Small C-inspired language that's transpiled down to C

named river 
cause it flows down to C ( badum tiss )

River is a programming language for those who like the DIY-nature of C, but sometimes wished that they had access to the convenient higher level features from other languages ( but are too scared of C++ ).
Simply put, river is a dialect of C with a boatload of syntax sugar and niceties piled on top, along with a side of an expressive type system and garnished with some really nice functional features to top it all off.

River is:
- Strongly typed
- 

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

(int, float) x = (23, 34.0);

int a;
float b;

// a gets set to x.0, b gets set to x.1
(a, b) = x;
```
## String Formatting
## Generics
## Algebraic Data Types
## Comptime code execution 
## Defers
The `defer` keyword just executes the given statement at the end of the current scope.

```c 

struct object;

{
    object x = new_object();
    defer delete_object;

    /*
     * do stuff ...
     */

    // calls delete here
}

```
## Ranges 

```c

range 0..10, 2
range 0..10:2
for (0 till 10 every 2) {

}

for ( 0 to 10) {

}

```

## For in / for each statements
```c 

for x in array {

}

```
## Default Arguments

```c 

struct object {
    int param1, 
        param2 = 30;
}

void function_definition ( int x, int y = 20, bool open = true ) {
    // define function over here
}

```

## Pattern matching & Destructuring
```c 
match user.name {
    "Sami" => {},
    "Hello" => {},
    _ => {}
};
```
## Closures 
```c
int x = () => {
    int x = 20;
    x += 12;
    x
}

x() // returns 12;
```

## Implicit Returns 
Expressions at the end of a block of code are implicitly returned.

```c 

int c () {
    int x = 0;
    x   // returns x
}

```

## Chaining methods 

a().b() means, function b takes whatever function a outputs as it's first argument;

```c 

let x = x()
        .y()
        .c()
        .t(4);
// C 

Type x = x(y(c(t(4))));

```

## Namespaces 

```c
namespace matrix {
    multiply(Vector v): Matrix {...}
}

namespace vector {
    multiply(Vector v): Vector {...}
}


// This

{
    using namespace matrix;

    let x = multiply(v);
}

let x = matrix:multiply(v);

// Translates to

{
    Matrix x = matrix_multiply(v);
}

Matrix x = matrix_multiply(v);

```


## easy struct matrix mapping:

consider a struct `Matrix`:
```c 
Struct Matrix {
    int col, row;
    float data[col*row];
};

```
typing 

```c 
Matrix m = {
    3 4 6 6 7
    2 4 2 5 7
    4 2 6 2 2
};
```

would map to 
```c 
Matrix m = (Matrix) {
    .col = 5, .row = 3,
    .data = {
        3, 4, 6, 6, 7,
        2, 4, 2, 5, 7,
        4, 2, 6, 2, 2
    }
}

```

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

struct object {...};

int main() => {
    object x = new_object();
    defer delete_object;

    /*
     * do stuff ...
     */

    // calls delete here
}

```

## For in / for each expressions
```c 

for x in array {

}

```

## Default values

```c 

struct object {
    int param1, 
        param2 = 30;
}

void function_definition ( int x, int y = 20, bool has_something = true ) {
    // define function over here
}

```

## Pattern matching & Destructuring
```c 
switch user.rank {
    case "Employee" => {},
    case "Admin" => {},
    _ => {}
};
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

A Small superset of C that's transpiled down to C, with lots of syntax sugar.

All ( or rather, most ) C code is valid river code, but river code is not at all valid C code.

named river
cause it flows down to C

Both a scripting language and a compiled one?

# Language Goals and Motivations
>   "Abstraction is layering ignorance on top of reality." -- Richard Gabriel

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

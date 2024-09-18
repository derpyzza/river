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

# Data Structures
## Hashmaps / Dictionaries
## Strings 
multiline strings
## Arrays
two array types: a regular C-style array, as well as a better array type
```c 

// Regular C style array. dissolves down into a pointer.
int array[4] = { 0, 1, 2, 3 };

// Better array type. holds the length
Array array(int, 4) = { 0, 1, 2, 3};

```

## Enums

new tagged enum type;
``` c
sum name {
    variant,
    variant,
    variant(string)
};
```
## Tuples

```c rvr
let x = (23, 59, f: "String");

```

```c 
struct tup_x x = struct {.0=23, .1=59, .f="string"};

```

# Language Features & Syntax Sugar
## Constructors and destructors
```c 

struct object {
    int x, y;
};

// object* obj = malloc(sizeof(object));
object obj = new object();

object* object(object* self, int x, int y) {
    self.x = 10;
}

// object() => { 10, 0 };
object obj = new object();

```
## String Formatting
## Comptime code execution 
## Rust style errors / Optional types / Result types
## Defers
The `defer` keyword just executes the given statement at the end of the current scope.

```c 
{
    object x = new object();
    defer delete object;

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

## Joint allocations?
https://github.com/BSVino/JaiPrimer/blob/master/JaiPrimer.md#proposed-features

## For in / for each statements
```c 

for x in array {

}

```
## Default Arguments
## Match statements 
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
    return 10;
}
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
## Variables 

```c 

// This
let x = 23; // implicitly int
let y = 20.; // or 20.0, 20f; all = 20.0


// To This
int x = 23;
float y = 23.0f;


```

## Classes

Classes are just a prettier struct

```cpp

// This
class ClassName {
    int i, j, k;
    bool x;

    new (int _i, bool _x) {
        _i = i;
        _x = x;
    }

    destroy () {

    }

    void func() {
        if self.x {
            10
        } else {
            4
        }
    }
}

ClassName c = new ClassName(3, 5);
c.func();

// Translates to 
struct ClassName {
    int i, j, k;
    bool x;
}

void func (ClassName* self) {
    if (self->x) {
        return 10;
    } else {
        return 4;
    }
}

ClassName c = new_class_name(3, 5);
func(&c);

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

    x := multiply(v);
}

x := matrix:multiply(v);

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

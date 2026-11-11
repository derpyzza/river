
- [ ]: finalize modules system
- [ ]: item visibility [ pub/priv ]
- [ ]: non-existence check operator ( either ?= or else )
  - as in, `let x = foo() ?= fallback_value`;
  - or, `let x = foo() else fallback_value`;
- [ ]: RTTI
- [ ]: constants
- [ ]: compile time code execution
- [ ]: Compiler attributes
- [ ]: Pattern matching
- [ ]: Lambdas / Closures?
- [ ]: Traits
- [ ]: Error handling
- [ ]: FFI
- [ ]: Variadic types
- [ ]: Generics


# tooling
- [ ]: Document comments

```rs

let file_data: [*]char;
read_file("/some/path.ext", fun(f) {
  file_data.fprintln(f);
});

with read_file("/some/path.ext") do fun(f) {
  file_data.fprintln(f);
};

let x = [_]int.{2, 3, 4, 5, 6};
let y = 4;

x.foreach( fun(_x) => _x * y );

```

```rs
while !game.should_quit() {
  perf_timer.time(game.render_frame());
} 

```

```c++

trait Animal {
  const Self: typeid;
  fun eat_food(self: Self);
}

struct Cat {
  ...
}

// trait implementation syntax?

fun Animal(Cat)::eat_food() {
  
}

```

should arrays and slices just follow generic syntax?
like, all generics use square braces for their params:
```rs

struct Hashmap[K, V: typeid] {
  // blah blah blah
}

let x: Hashmap[string, int];
let y: array[10, f32]; // array of size 10, of f32s
let z: slice[f32];
let q: array[20, array[20, ptr[int]]]; // multi-dimentional arrays of pointers to ints
  

let e: [10]Hashmap[string, int];
let e: [10]Hashmap{string, int};
let e: [10]Hashmap<string, int>;
let e: [10]Hashmap::<string, int>;

let arr: int[9]; 
let arr: *int[9]; 
let arr: int*[9]; 
let arr: *int*[9]; 
```


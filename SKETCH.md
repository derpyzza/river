for a hypothetical programming language, i'd have the following file structure:

```
mylang
|- main.rvr [ setup and entrypoint ]
|- enums.rvr [ common enums: tokens, asts, etc ]
|- scanner.rvr
|- parser.rvr
|- interpreter.rvr

```

with the import heirarchy looking something like:
```
           +->[parser.rvr]
[main.rvr]-| 
           +->[interpreter.rvr]

             +->[enums.rvr]
[parser.rvr]-|
             +->[scanner.rvr]

[scanner.rvr]->[enums.rvr]
[interpreter.rvr]->[enums.rvr?] ( probably )
```

do i want code to look like this:
```rs
module mylang;

import std::io;

fun main(args: []string) {
  for arg in args {
    let file, err := io.fread(arg) else {
      io::print_err(err);
      continue;
    };

    let ast, err := parse(file) else {
      io::print_err(err);
      continue;
    }

    io::print(interpret(ast));
  }
}
```

or this:

```rs
module main;

import package::parser;
import package::interpreter;
import std::io;

fun main(args: []string) {
  for arg in args {
    let file, err := io::fread(arg) else {
      io::print_err(err);
      continue;
    };

    let ast, err := parser::parse(file) else {
      io::print_err(err);
      continue;
    }

    io::print(interpreter::interpret(ast));
  }
}
```

i'd like to structure a code
```

```

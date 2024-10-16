required tooling:
    compiler [ in progress ]
    repl 
    docgen
    test framework
    build system
    syntax highlighting
    intellisense
    stdlib

maybe tooling:
    interpreter
    package manager


# General
-[ ] better error handling, with prettier errors ( maybe keep a separate `Diagnostics` module for error displaying, like you've seen other languages do? )
-[ ] better memory handling, consider using some sort of memory pool or arena allocator where you allocate a bit fat chunk of memory at once and then distribute pointers to positions in the memory.

# Lexer
-[x] Allow for floating var scanning
-[ ] Allow for `d` or `f` prefixes to floating point literals
-[ ] Allow for nested block comments
-[ ] Check for doc comments
-[ ] Implement parsing for alternate number literal styles:
    -[ ] hex literals ( 0xBEEF )
    -[ ] octal literals ( 0c32 )
    -[ ] binary literals ( 0b110101 )
    -[ ] number separators ( 12_000 and 12'000 )

# Parser
-[x] Get started with the parser
-[x] Parse a simple arithmetic expression

-[ ] Implement better panic handling, something that is more robust and expressive. 
     The current system has a multitude of problems, namely:
     The stop points for panic.until are really rigid, currently you can either chose to stop at a token, or at a cat.
     Ideally, you'd want it such that it takes a variable number of tokens, and iterate through all of them and check to see if any of them match.
         > Ok that's not exactly a "multitude" but it is annoying enough to feel like it.
    
-[ ] add support for undefined values
-[ ] add parsing for types other than int
-[ ] in the case of types other than int, use the appropriate literal types for them
-[ ] add post fix operators ( `++` and `--` )
-[ ] implement tuple types
-[ ] implement pointer types
-[ ] implement array types
-[ ] implement array literals
-[ ] implement tuple literals
-[ ] implement block expression parsing
-[ ] implement the nullable `?` operator

-[ ] implement the remaining levels of expression parsing:
    -[ ] grouping
    -[ ] comparison
    -[ ] assignment
    -[ ] binary operations

-[ ] implement if expression parsing ( along with the associated else, then clauses )
    -[ ] basic if
    -[ ] else
    -[ ] then

-[ ] implement while expression parsing
    -[ ] inline
    -[ ] block

-[ ] implement for loop parsing
    -[ ] c style
    -[ ] for each
    -[ ] inline
    -[ ] block

-[ ] implement switch/match expression parsing
    -[ ] case parsing
    -[ ] multiple case parsing
    -[ ] default case parsing



# Notes and Junk

what if the doc comments embedded markdown in them?
\+ what if the markdown supported inline la/tex? that'd be pretty cool... 


doc comments are contained in `//# ... #//` blocks:
```
//#
# Parser
The main parser struct, which tracks the current parsing state 

#//
struct Parser {
    /* ... */
}
 
```

>!![BOSS EVENT]!!
> memory handling in this compiler is actually messed up. fix that sometime.



need to setup a nicer error reporting setup in the parser. the current one is pretty nice as far as primitive error reporting goes but it could be better.
here's a list of things that'll make it nicer:
- Coloured text
- have the error print the line of source code where the error occurred along with the error itself:
    in.rvr:10:10: ERROR Unexpected token; expected identifier, got "if" (if) instead
    | 10 |  let if = i_frame();

- point out where the error is, in the line of source code itself using a pretty printed error pointer.
    in.rvr:10:10: ERROR Unexpected token; expected identifier, got "if" (if) instead
    10 |  let if = i_frame();
       |      ^^
- categorize tokens into different categories, and include the categories into the error message.
    i.e:
    This:
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got "if" (if) instead
    Becomes:
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got Keyword "if" instead;
        10 |  let if = i_frame();
           |      ^^
- Give helpful hints / alternatives if possible:
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got Keyword "if" instead;
        10 |  let if = i_frame();
           |      ^^
        hint: consider renaming "if" to a different name if it's a variable, e.g "_if";

BUGS LIST:
- the panic function should be able to take in a range of stop points, as right now it can completely miss certain errors on separate lines
| due to the line not having the expected terminator. 
| i.e:
```
1 | import std. // <- Error: missing identifier.
```
This will skip all tokens until it encounters the next semicolon. 
but notice how this line itself also doesn't have a semi colon.

```
2 | fn void func => do_something(); 
```
this is completely valid code, but it ends with a semicolon. the panic function from the
previous error will completely skip over this entire function, and stop panicking once import
reaches the semi colon.

Now in this case it works out fine, but consider if line 2 was like this:
```
2 | fn void func do_something(); // Error: missing '=>'
```
Now here, the line still ends with a semi colon which means that the panic set off from the
previous line's error will terminate itself here, but in doing so, it will completely ignore the
missing arrow error. The missing arrow error will then only show itself to the user when they've gone
back to fix the error on line 1, only to be met with another error on line 2. This can also snowball
into a mountain of tiny errors that only reveal themselves once the previous one is fixed, which makes the
whole process of error handling a HUGE chore.

The solution then, would be to add a bunch of extra checks for the panic mode to terminate itself.
For example, when parsing a statement, upon encountering an error the parser could either stop panicking upon
encountering a semi-colon, but in case the semi colon is also missing, it could then look to see if it can detect
the beginning of a new statement, marked by a set of specific tokens which are known to start a new statement;
i.e 'if', 'fn', 'struct', 'import', etc.

ERROR HANDING
===

-[] input int too large


# ROADMAP

-[IN PROGRESS] Version 0: working river-to-c-compiler that fully implements the basic river spec
-[] Version 1: Simple stdlib: io, string, math, vector, hashmap, sort, json log etc.
-[] Version 2: Package management, documentation generation, etc.

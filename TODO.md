-[x] Allow for floating var scanning

-[x] Get started with the parser
-[x] Parse a simple arithmetic expression

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


required tooling:
    compiler
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

-[] Version 0: working river-to-c-compiler that fully implements the basic river spec
-[] Version 1: Simple stdlib: io, string, math, vector, hashmap, sort, json log etc.
-[] Version 2: Package management, documentation generation, etc.

## Scanner
-[] Implement proper parsing for floating point literals to end with an 'f' indicating a float type and a 'd' indicating a double type
-[] Implement separate multiline strings

## Parser


implement module importing

implement variable assignment
implement multiple variable assignment

implement structs
implement tuples

FUNCTIONS:
- Implement function different return types
- Implement function parameters
- implement explicit function returns
- implement function calling
- implement uniform function call syntax


## Type checking

## Codegen


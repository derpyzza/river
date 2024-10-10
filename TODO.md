-[x] Allow for floating var scanning

-[x] Get started with the parser
-[x] Parse a simple arithmetic expression

>!![BOSS EVENT]!!
> memory handling in this compiler is actually messed up. fix that sometime.


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


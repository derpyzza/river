-[x] Allow for floating var scanning

-[x] Get started with the parser
-[x] Parse a simple arithmetic expression

>!![BOSS EVENT]!!
> memory handling in this compiler is actually messed up. fix that sometime.


currently the error reporting is being done by the parse_ast function,
whilst the parse functions just simply check for and return the errors.

I think that the parser struct should actually keep a track of all errors, and have access to them at all times,
rather than having to reach into nodes and check them for errors.
maybe if an error is encountered in a parse node function, it can abort parsing that node and return a node of type N_ERROR or something, and
register the error ( with what went wrong, expected / got tokens, source + offsets, whatever included in it ), and set off a "had_error" flag.
then, before returning the parse tree, the parser could check if the had_error flag has been set, and if so, go through it's list of errors and
report each one to the user before terminating the program, instead of moving on.

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


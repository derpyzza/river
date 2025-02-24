


language features:
    - Basic Features:
        - variables
        - functions
        - default function arguments
        - if/else expressions
        - while expressions
        - repeat until expressions
        - type defs / aliases
        - structs
        - enums
        - unions
    - 'Advanced Features':
        - operator overloading
        - function overloading
        - Type Inference
        - Comptime macros/metaprogramming ( see nim/lisp/rust style macros )
        - Generics
        - Interfaces


note:
    make the river compiler ( or at the very least, just the parser and codegenerator ) available within river code as a library, 
    so that parsing and generating river code from river code is ezpz
    imagine being able to generate a bunch of boilerplate structs and functions and whatnot using a river program :O
    no more need for python!

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


bare minimum functional river compiler:

functions:
    TYPE NAME ( PARAMS ) => BODY

Types:
    int, float, str, char, bool, void ( for functions );
    variables are typechecked

Variable stuff:
    let
    modifiers:
        mut, ^, []

function bodies are flat list of variable declarations and expressions and function calls

supports imports and moduling

supports C foreign function importing ( for printf );

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

river should probably be able to figure out struct alignment and place the fields in the most optimized alignment.

How about adding semantic casing?
constants in all caps:
    const Int SOMETHING = 10;
function and variable names in snake_case:
    Bool is_false = true;
    Void do_something() = {
        println("Something");
    }
private variables begin with an underscore:
    Int _private = 10;
types, structs, enums and unions should all be written in PascalCase:
    Int, Byte, Short, Long, UInt, ULong, UShort, UByte,
    Char, Str, Float, Double, Bool, Void.

grouping syntax:

{} = blocks, structs;
() = parenthesized expressions, tuples
[] = array subscripts, array literals, generics


struct literals cannot be anonymous:
 STRUCTNAME {}
struct literal fields MUST be named.

structs are nominal types that hold named values of different types.
tuples are structural types that hold unnamed values of different types.
arrays are structural types that hold unnamed values of the same type.

for (_, ^x) in intList do x *= x; 

@ for pointer derefencing

```c

int x = 10
    y = 23
    z = 25;

    
```



```
(if x = 10 then 3 else 7) * 4; // valid

int[10] list= [ 23, 53, 46, 68, 35, 57, otherList... ];

10 * ( 12 + 4 ) // syntax and semantics OK.
12 * ( 12, "string" ); // syntactically OK, semantically errors.

```

let x = 10 * if y then z else d;



perhaps function parameters should be tuples, with functions only taking one parameter?
it can be useful for when you have many functions that do different things but have the same type signature:
```c
// this
Vec translateX (Vec *v, int by) => { /*...*/ }
Vec translateY (Vec *v, int by) => { /*...*/ }
Vec translateZ (Vec *v, int by) => { /*...*/ }

// to this
type translateParams -> (^Vec v, int by);

Vec translateX translateParams => { /*...*/ }
Vec translateY translateParams => { /*...*/ }
Vec translateZ translateParams => { /*...*/ }    
```

counter point: the same can be done via structs too:
```c
struct translateParams {
    ^Vec v;
    int by;
};

Vec translateX(translateParams params) => { /*..*/ }
Vec translateY(translateParams params) => { /*..*/ }
Vec translateZ(translateParams params) => { /*..*/ }
```

the only difference then, would be on the caller's side:
```c
// struct
translateX({v, by});

// tuple
translateX (v, by);
```

though do note that for the struct version to work you'll have to implement some sort of
anonymous object literal syntax so that you can define a struct literal without having 
to insert a compound literal.

and to pull *that* off i'll need to figure out whether or not to go the `nominal` or `structural` 
type system routes. The reason being that currently river is designed with a nominal type
system in mind. What that means in short is that the following two structs are not equivalent:
```c
struct Colour {
    int r, g, b;
}; 

struct Vec3 {
    int x, y, z;
};

struct Colour c = { 20, 30, 40 };
struct Vec3 v = { 20, 30, 40 };
```
even though they take up the same exact amount of memory and have the same exact field values,
they are counted as being two separate entities entirely, because entities ( or objects or types or whatever )
are compared on a by-name basis. So even though two types may be *structurally* equivalent, if the types have different
names they are treated as separate types and can not be interchanged at all. This is of course, very useful.
For instance, you wouldn't want to accidentally pass a `Colour` to a function that expects a `Vec3` and vice versa.
However, it does make calling and defining other types of functions a lot harder. For instance:

```c

struct ThreeValues {
    int x, y, z;
}

int SumThreeValues(struct ThreeValues v) => v.x + v.y + v.z;

Vec3 v = { 20, 30, 40 };
SumThreeValues((ThreeValues){v.x, v.y, v.z});
```

In order to have general functions like `SumThreeValues` which takes in a set number of values,
you have to manually destructure your struct into the target type of struct, which is quite annoying.
In situations like this it'd be nice to let the language automatically destructure the struct, or even have
a keyword that destructures the struct to the required params if the struct meets the required criteria.
This sort of pattern matching btw, should also apply for tuples and slices.

also what of a batch of functions that takes in almost the same parameters, save for a few differences?
see for example:

```c

void TokenInt(int i, TokenType t, Position pos, Source s);
void TokenChar(char c, TokenType t, Position pos, Source s);
void TokenString(string s, TokenType t, Position pos, Source s);    

```
these could be modelled in several different ways.
For example, They could be member functions of a class that tracks the other state and the functions just extract it from there
but what if river had something like array comprehension in python?

```c

type TokenParams -> (TokenType t, Position pos, Source s);

void TokenInt(int i, ..TokenParams);
void TokenChar(char c, ..TokenParams);
void TokenString(string s, ..TokenParams);    
```

This way, you could avoid duplication. But then again, this could also be modelled with a struct:

```c

struct TokenParams {
    TokenType t;
    Position pos; 
    Source s;
}
    
```


`?=` operator as a shorthand for `if x then x else y`
```
int len = if s.len() != null then s.len() else 0; 

int len = s.len() ?= 0;
```

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

- Some sort of compiler debug mode that prints the compiler source file and line that the error emanates from. ( this is purely for me, and not for the user ( which is also, me ))
        [DEBUG]: error emitted from <src/parser/parser.c:123>
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got Keyword "if" instead;
        10 |  let if = i_frame();
           |      ^^
        hint: consider renaming "if" to a different name if it's a variable, e.g "_if";
- Errors should have a unique error code to aid localization.
- Errors should display the name of the parent function of the block of code in which an error occurred, if it occurs in a function body.
        [DEBUG]: error emitted from <src/parser/parser.c:123>
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got Keyword "if" instead;
        in function onDodge()
        10 |  let if = i_frame();
           |      ^^
        hint: consider renaming "if" to a different name if it's a variable, e.g "_if";
- Examples of correct syntax ( Optional though, might be a pain to implement )
        [DEBUG]: error emitted from <src/parser/parser.c:123>
        in.rvr:10:10: ERROR Unexpected token; expected identifier, got Keyword "if" instead;
        in function onDodge()
        10 |  let if = i_frame();
           |      ^^
        hint: consider renaming "if" to a different name if it's a variable, e.g "_if";
        \>    let _if = i_frame();
- Link to relevant docs ( Displayed optionally )
- For wrong values in function calls, print the type signature of the function definition and the type signature of the erroneous function call:
    Error: Unexpected value of type <int> where <string> was expected:
        fn_call(14, 14); // => fn_call( int, int )
        Expected: fn_call( int, string, int ) 

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

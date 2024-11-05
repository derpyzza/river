#pragma once

#include <stdlib.h>
#include <stdint.h>

#define True ((0==0))
#define False ((0!=0))

#define in_range_inc(v, min, max) ((min <= v) && (v <= max))
#define in_range_ex(v, min, max)  ((min < v) && (v < max))

typedef intptr_t size;
typedef uintptr_t usize;
typedef uint32_t bool;
typedef uint32_t uint;
typedef uint64_t ulong;
// kinda ugly, i'd much rather just use long
// but the problem is when i want a number bigger than int:
// on my machine, sizeof long == int, which means i have to type long long int
// which is stupid.

// i don't *really* like the concept of sized types too much in the context of normal programs
// ideally you should just use the default int type unless you
// REALLY need to use some different type ( i.e in an embedded context, or perhaps you're developing a virtual machine )
// short ( or u/i16 ) specifically is generally pretty useless for normal applications,
// and i don't see myself needing a short for this app so it's being left out.
typedef int64_t ilong;

// i'm defining this jic but idk if i'm gonna end up using this
typedef struct Result {
	int err;
	void *ok;
} Result;

// I bet a compiler would have a lot of use for string manipulation stuff
// this struct should be moved into it's own module in that case.
typedef struct String {
	int len; // not size, because the string format expects an int as the length
	int cap;
	char* c_ptr;
} String;

typedef struct File {
	bool is_valid;
	char* path;
	String data;
} File;

typedef struct Vec {
	size max;
	size current;
	void** data;
} Vec;

#define vec_of(type, size) new_vec(size, (sizeof(type)))

Vec *new_vec(size init, size elem_size);
Vec *init_vec(void** data, size len);
void vec_push(Vec *v, void* item);
void vec_pushi(Vec *v, int i);
void vec_pop(Vec *v);

String *str_from(const char* s);
void str_append(String* o, char* s);

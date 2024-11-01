#pragma once

#include <stdlib.h>


// I bet a compiler would have a lot of use for string manipulation stuff
// this struct should be moved into it's own module in that case.
typedef struct String {
	size_t len;
	char* c_ptr;
} String;

typedef struct File {
	int is_valid;
	char* path;
	String data;
} File;

typedef struct Vec {
	int max, current;
	void** data;
} Vec;

#define vec_of(type, size) new_vec(size, (sizeof(type)))

Vec *new_vec(size_t init, size_t elem_size);
Vec *init_vec(void** data, long len);
void vec_push(Vec *v, void* item);
void* vec_pop(Vec *v);

#define in_range_inc(v, min, max) ((min <= v) && (v <= max))
#define in_range_ex(v, min, max)  ((min < v) && (v < max))

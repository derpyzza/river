#pragma once

#include <stdlib.h>
typedef struct Vec Vec;

struct Vec {
	int max, current;
	void** data;
};

#define vec_of(type, size) new_vec(size, (sizeof(type)))

Vec *new_vec(size_t init, size_t elem_size);
Vec *init_vec(void** data, long len);
void vec_push(Vec *v, void* item);
void* vec_pop(Vec *v);

#define in_range_inc(v, min, max) ((min <= v) && (v <= max))
#define in_range_ex(v, min, max)  ((min < v) && (v < max))

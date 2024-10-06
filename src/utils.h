#pragma once

typedef struct Vec Vec;

struct Vec {
	int max, current;
	void** data;
};

Vec *new_vec(void);
Vec *init_vec(void** data, long len);
void vec_push(Vec *v, void* item);
void* vec_pop(Vec *v);

#include "utils.h"
#include <stdlib.h>
#include <string.h>

Vec *new_vec(size_t init, size_t elem_size) {
	Vec *v = malloc(sizeof(Vec));
	v->max = init;
	v->current = 0;
	v->data = calloc(v->max, sizeof(elem_size));
	return v;
}

Vec *init_vec(void** data, long len) {
	Vec *v = new_vec(len, sizeof(void*));
	memcpy(data, v->data, len);
	return v;
}

void vec_push(Vec *v, void* item) {
	if (v->current + 1 > v->max) {
		v->max *= 2;
		v->data = realloc(v->data, v->max);
	}

	v->data[v->current] = item;
	v->current++;
}

void *vec_pop(Vec *v){
	if (v->current == 0) {
		return 0;
	}

	return v->data[--v->current];
}

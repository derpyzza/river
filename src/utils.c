#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Vec *new_vec(void) {
	Vec *v = malloc(sizeof(Vec));
	v->max = 32;
	v->current = 0;
	v->data = malloc(sizeof(void*) * v->max);
	memset(v->data, 0, v->max);
	return v;
}

Vec *init_vec(void** data, long len) {
	Vec *v = new_vec();
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

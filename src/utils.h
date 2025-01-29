#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define IN_RANGE_INC(v, min, max) ((min <= v) && (v <= max))
#define IN_RANGE_EX(v, min, max)  ((min < v) && (v < max))

#define NULL_PTR_PANIC(ptr) if (ptr == NULL)	\
printf("NULL POINTER DEREFERENCE EXCEPTION AT %s, %i\n", __FILE__, __LINE__),	\
exit(-1);

typedef unsigned int uint;
typedef intptr_t size;
typedef uintptr_t usize;
typedef uint64_t ulong;
typedef int64_t ilong;

typedef struct String {
	int len; // not size, because the string format expects an int as the length
	int cap;
	char* c_ptr;
} String;

String *str_new(size init);
String *str_from(const char* s);
void str_append(String* o, char* s);

// linked list of strings
typedef struct StringBuilder {

	String string;
	String *next; // pointer to next string in list
	
} StringBuilder;

typedef struct File {
	bool is_valid;
	char* path;
	String data;
} File;

typedef struct FilePath {
	char* path;
	char* ext;
} FilePath;

File *read_file(char* path);
FilePath split_path(char* path);

typedef struct Vec {
	size max;
	size current;
	void** data;
} Vec;

Vec *new_vec(size init, size elem_size);
Vec *init_vec(void** data, size len);
void vec_push(Vec *v, void* item);
void vec_pushi(Vec *v, int i);
void vec_pop(Vec *v);

// evil preprocessor macro hack
// declares a type-safe dynamic array, along with all the functions required to push data into and pull data from it.
// T is the base type, I is the name for the outputted datatype
#define CREATE_VEC_TYPE(T, I)																					\
	typedef struct Vec##I {				 																			\
		T *data; 																													\
		size current, max;																								\
	} Vec##I;																														\
																																			\
	static inline Vec##I *new_vec_##I (size init) {											\
		Vec##I *v = (Vec##I*)malloc(sizeof(Vec##I));											\
		if (v) { 																													\
			v->max = init; 																									\
			v->current = 0;																									\
			v->data = ( T* )calloc(v->max, sizeof( T ));										\
		}																																	\
		return v;																													\
	}																																		\
																																			\
	static inline void vec_grow_##I (Vec##I *v, size by){ 							\
		if (v->current + 1 > v->max) {																		\
			v->max *= 2;																										\
			v->data = (T*)realloc(v->data, sizeof(T) * v->max);							\
		}																																	\
	}																																		\
																																			\
	static inline void vec_push_##I (Vec##I* v, T i) {	 								\
		if (v->current + 1 > v->max) { 																		\
			v->max *= 2; 																										\
			v->data = (T*)realloc(v->data, sizeof(T) * v->max); 						\
		} 																																\
		v->data[v->current] = i; 																					\
		v->current++; 																										\
	}																																		\
	static inline T *vec_pop_##I (Vec##I* v) {													\
		return (T *)&v->data[(--v->current)];															\
	}

#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
	@param init — initial array size
	@param elem_size — size of one element
*/
Vec *new_vec(size init, size elem_size) {
	Vec *v = (Vec*)malloc(sizeof(Vec));
	if(v) {
		v->max = init;
		v->current = 0;
		v->data = (void**)calloc(v->max, sizeof(elem_size));
		return v;
	}

	else printf("Error: Not enough memory to create new vec\n"), exit(-1);
}

/** 
	@param data — Initial data to start out with
	@param len — length of array
*/
Vec *init_vec(void** data, size len) {
	Vec *v = new_vec(len, sizeof(void*));
	memcpy(data, v->data, len);
	return v;
}

void vec_push(Vec *v, void* item) {
	if (v->current + 1 > v->max) {
		v->max *= 2;
		v->data = (void**)realloc(v->data, sizeof(void*) * v->max);
	}

	v->data[v->current] = item;
	v->current++;
}

void vec_pushi(Vec *v, int i) {
	vec_push(v, (void *)(size)i);
}

void _vec_pop(Vec *v){
	if (v->current == 0) {
		return;
	}

	v->data[--v->current] = NULL;
	return;
}

String *str_new(size init) {
	String *str = (String*)malloc(sizeof(String));

	str->len = init;
	str->c_ptr = (char*)malloc(sizeof(char) * str->len);

	if(str) return str;
	else return NULL;
}

String *str_from(const char *s) {
	String *str = (String*)malloc(sizeof(struct String));
	str->len = str->cap = strlen(s);
	str->c_ptr = (char*)malloc(sizeof(char) * str->len);
	if (memcpy(str->c_ptr, s, str->len)) return str;
	else printf("Error: Could not copy memory for new string"), exit(-1);
}

void str_grow(String* s, size len) {
	if (s->len + len < s->cap ) return;

	while ( s->len + len > s->cap )
		s->cap *= 2;
	s->c_ptr = (char*)realloc(s->c_ptr, sizeof(char) * s->cap);
}

void str_append(String* o, char* s) {
	size len = strlen(s);
	str_grow(o, len);

	memcpy(o->c_ptr + o->len, s, len);
	o->len += len;
}

StringBuilder * sb_new(const char * src) {
	StringBuilder *sb = (StringBuilder*)malloc(sizeof(*sb));
	sb->string = *str_from(src);
	sb->next = NULL;
	return sb;
}


File *read_file(char* path) {
	File *out = (File*)malloc(sizeof(File));
	out->path = path;
	out->is_valid = false;

	FILE *in = fopen(path, "rb");
	if (in == NULL) {
		printf("Error, could not read file %s\n", path);
		return out;
	}
	fseek(in, 0, SEEK_END); 
	out->data.len = ftell(in); 
	rewind(in); 

	out->data.c_ptr = (char*)malloc(out->data.len + 1);
	if (out->data.c_ptr == NULL) {
		printf("Error, could not allocate enough memory for file %s\n", path);
		fclose(in);
		return out;
	}

	size bufsread = fread(out->data.c_ptr, 1, out->data.len, in);
	if ( bufsread < out->data.len ) {
		printf("Error, could not read enough bytes from file %s\n", path);
		fclose(in);
		return out;
	}

	fclose(in);
	// everything went fine
	out->is_valid = true;
	return out;
}

FilePath split_path(char* path) {
	size len = strlen(path);
	char* name = (char*)malloc(sizeof(char) * len);
	int id = 0;
	char* c = path;
	while(*c != '.') {
		name[id] = *c;
		id++;
		c++;
	}
	name[id] = '\0';

	printf("size is %li\n", (len - id) * sizeof(char) + 1);
	char* ext = (char*)malloc((len - id) * sizeof(char) + 1);
	if(ext != NULL) 
		strcpy(ext, c);
	else printf("Error: Could not malloc ext file\n"), exit(-1);
	return (FilePath){ .path = name, .ext = ext };
}


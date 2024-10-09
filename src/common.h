#pragma once
#include <stdlib.h>

// I bet a compiler would have a lot of use for string manipulation stuff
// this struct should be moved into it's own module in that case.
typedef struct string_s {
	size_t size;
	char* string;
} string_s;

typedef struct src_file_s {
	int is_valid;
	char* path;
	string_s data;
} file_s;


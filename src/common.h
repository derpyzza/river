#pragma once
#include <stdlib.h>

typedef struct src_file_s {
	int is_valid;
	size_t size;
	char* data;
	char* path;
} file_s;

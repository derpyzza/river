#pragma once
#include <libderp/derp.h>

// memory management shenanigans
//

// virtual memory
typedef struct vmem {
  u8* buf;  
  size allocated, size;
} vmem;

void   vmem_init  (vmem *buf, size mb_size);
void * vmem_alloc (vmem *buf, size len);
void   arena_free (vmem *buf);

void   memory_init (void);
void * alloc_arena (size);
void * free_arena  (size);
void * calloc_mem  (size);
void * calloc_str  (size);

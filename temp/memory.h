#pragma once
#include <libderp/derp.h>

// memory management shenanigans
//

// virtual memory
typedef struct vmem {
  u8* buf;  
  isize allocated, size;
} vmem;

void   vmem_init  (vmem *buf, isize mb_size);
void * vmem_alloc (vmem *buf, isize len);
void   arena_free (vmem *buf);

void   memory_init (void);
void * alloc_arena (isize);
void * free_arena  (isize);
void * calloc_mem  (isize);
void * calloc_str  (isize);

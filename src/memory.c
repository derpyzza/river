#include <libderp/ddebug.h>
#include <sys/mman.h>
#include <errno.h>
#include "memory.h"

void
mmap_init(vmem * mem, size len) {
  void * ptr = 0;
  #if PLATFORM_WINDOWS

  #elif PLATFORM_POSIX
    size min_size = len / 16;
    if(min_size < 1) min_size = len;
    while(len >= min_size) {
      ptr = mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
      // success?
      if(ptr != MAP_FAILED && ptr) break;
      // did it fail in a non-retriable way?
      		if (errno != ENOMEM && errno != EOVERFLOW && errno != EAGAIN) break;
  		// Try a smaller size
  		len /= 2;  
    }

    if((ptr == MAP_FAILED) || !ptr) {
      dlog_error("Error initializing memory");
    }
  #else
    // PANIC
  #endif
  mem->size = len;
  mem->buf = ptr;
  mem->allocated = 0;
}

void *
mmap_alloc(vmem* mem, size alloc) {
  size alloc_after = alloc + mem->allocated;

  #if PLATFORM_WINDOWS

  #endif

  void* ptr = ((u8*)mem->buf) + mem->allocated;
  mem->allocated = alloc_after;
	if (mem->size < alloc_after) {
		dlog_error("Error: The compiler ran out of memory! Over %u MB was allocated from a single memory vmem, perhaps you called some recursive macro?", (unsigned)(mem->size / (1024 * 1024)));
		return 0;
		//error exit
	}
	return ptr;
}

size max = 0x10000000;

void
vmem_init(vmem * buf, size mb_size) {
  if(mb_size > max) mb_size = max;
  mmap_init(buf, 1024 * 1024 * mb_size);
}

void *
vmem_alloc(vmem* mem, size size) {
  return mmap_alloc(mem, size);
}

static vmem mem_arena = {0};
static vmem char_arena = {0};

void
memory_init() {
	vmem_init(&mem_arena, 2048);
	vmem_init(&char_arena, 512);
}

void *
calloc_mem(size mem) {
  ASSERT(mem > 0, "")

  mem = (mem + 15U) & ~15ULL;
  return vmem_alloc(&mem_arena, mem);
}

void *
calloc_str(size len) {
  ASSERT(len > 0, "")

  return vmem_alloc(&char_arena, len);
}

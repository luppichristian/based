// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/vmem.h"
#include "basic/env_defines.h"
#include "basic/utility_defines.h"
#include <string.h>

// =========================================================================
// Platform Implementations
// =========================================================================

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

func sz vmem_page_size(void) {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return (sz)info.dwPageSize;
}

func void* vmem_reserve(sz size) {
  return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

func b32 vmem_commit(void* ptr, sz size) {
  return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL ? 1 : 0;
}

func b32 vmem_decommit(void* ptr, sz size) {
  return VirtualFree(ptr, size, MEM_DECOMMIT) != 0 ? 1 : 0;
}

func b32 vmem_release(void* ptr, sz size) {
  (void)size;
  return VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? 1 : 0;
}

func void* vmem_platform_alloc(sz size) {
  return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

func b32 vmem_platform_free(void* ptr, sz size) {
  (void)size;
  return VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? 1 : 0;
}

#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)

#  include <sys/mman.h>
#  include <unistd.h>

func sz vmem_page_size(void) {
  return (sz)sysconf(_SC_PAGESIZE);
}

func void* vmem_reserve(sz size) {
  void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    return NULL;
  }
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0 ? 1 : 0;
}

func b32 vmem_decommit(void* ptr, sz size) {
  if (mprotect(ptr, size, PROT_NONE) != 0) {
    return 0;
  }
  // Hint to the OS to release the physical pages; best-effort, not checked.
  (void)madvise(ptr, size, MADV_DONTNEED);
  return 1;
}

func b32 vmem_release(void* ptr, sz size) {
  return munmap(ptr, size) == 0 ? 1 : 0;
}

func void* vmem_platform_alloc(sz size) {
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    return NULL;
  }
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  return munmap(ptr, size) == 0 ? 1 : 0;
}

#else

// =========================================================================
// Fallback — platforms without virtual memory primitives
// =========================================================================
// reserve + commit collapse into a single malloc; decommit is a no-op.

#  include <stdlib.h>

func sz vmem_page_size(void) {
  return 4096;
}

func void* vmem_reserve(sz size) {
  return malloc(size);
}

func b32 vmem_commit(void* ptr, sz size) {
  (void)ptr;
  (void)size;
  return 1;
}

func b32 vmem_decommit(void* ptr, sz size) {
  (void)ptr;
  (void)size;
  return 1;
}

func b32 vmem_release(void* ptr, sz size) {
  (void)size;
  free(ptr);
  return 1;
}

func void* vmem_platform_alloc(sz size) {
  return malloc(size);
}

func b32 vmem_platform_free(void* ptr, sz size) {
  (void)size;
  free(ptr);
  return 1;
}

#endif

// =========================================================================
// Allocation Wrappers
// =========================================================================

typedef union vmem_alloc_header {
  struct {
    sz mapping_size;
    sz prefix_size;
    sz user_size;
  } info;
  max_align_t force_align;
} vmem_alloc_header;

func vmem_alloc_header* vmem_get_alloc_header(void* ptr) {
  return ((vmem_alloc_header*)ptr) - 1;
}

func void* vmem_get_alloc_base(void* ptr) {
  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  return (u8*)ptr - header->info.prefix_size;
}

func sz vmem_get_alloc_align(void) {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  return vmem_page_size();
#else
  return align_of(max_align_t);
#endif
}

func void* vmem_alloc(sz size) {
  if (size == 0) {
    return NULL;
  }

  sz alloc_align = vmem_get_alloc_align();
  sz prefix_size = align_up(sizeof(vmem_alloc_header), alloc_align);

  if (size > SZ_MAX - prefix_size) {
    return NULL;
  }

  sz total_size = prefix_size + size;
  void* base_ptr = vmem_platform_alloc(total_size);
  if (!base_ptr) {
    return NULL;
  }

  u8* user_ptr = (u8*)base_ptr + prefix_size;
  vmem_alloc_header* header = ((vmem_alloc_header*)user_ptr) - 1;
  header->info.mapping_size = total_size;
  header->info.prefix_size = prefix_size;
  header->info.user_size = size;
  return user_ptr;
}

func void* vmem_calloc(sz count, sz size) {
  if (count == 0 || size == 0) {
    return NULL;
  }

  if (count > SZ_MAX / size) {
    return NULL;
  }

  sz total_size = count * size;
  void* ptr = vmem_alloc(total_size);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  return ptr;
}

func void* vmem_realloc(void* ptr, sz old_size, sz new_size) {
  (void)old_size;
  if (!ptr) {
    return vmem_alloc(new_size);
  }

  if (new_size == 0) {
    (void)vmem_free(ptr, 0);
    return NULL;
  }

  vmem_alloc_header* old_header = vmem_get_alloc_header(ptr);
  if (new_size == old_header->info.user_size) {
    return ptr;
  }

  void* new_ptr = vmem_alloc(new_size);
  if (!new_ptr) {
    return NULL;
  }

  sz copy_size = old_header->info.user_size < new_size ? old_header->info.user_size : new_size;
  memcpy(new_ptr, ptr, copy_size);
  (void)vmem_free(ptr, 0);
  return new_ptr;
}

func b32 vmem_free(void* ptr, sz size) {
  (void)size;
  if (!ptr) {
    return 1;
  }

  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  return vmem_platform_free(vmem_get_alloc_base(ptr), header->info.mapping_size);
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* vmem_alloc_callback(void* user_data, callsite site, sz size) {
  (void)user_data;
  (void)site;
  return vmem_alloc(size);
}

func void vmem_dealloc_callback(void* user_data, callsite site, void* ptr) {
  (void)user_data;
  (void)site;
  (void)vmem_free(ptr, 0);
}

func void* vmem_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  (void)user_data;
  (void)site;
  return vmem_realloc(ptr, old_size, new_size);
}

func allocator vmem_get_allocator(void) {
  allocator alloc;
  alloc.user_data = NULL;
  alloc.alloc_fn = vmem_alloc_callback;
  alloc.dealloc_fn = vmem_dealloc_callback;
  alloc.realloc_fn = vmem_realloc_callback;
  return alloc;
}

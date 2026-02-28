// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/vmem.h"
#include "basic/env_defines.h"

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

func void* vmem_alloc(sz size) {
  return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

func b32 vmem_free(void* ptr, sz size) {
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

func void* vmem_alloc(sz size) {
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    return NULL;
  }
  return ptr;
}

func b32 vmem_free(void* ptr, sz size) {
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

func void* vmem_alloc(sz size) {
  return malloc(size);
}

func b32 vmem_free(void* ptr, sz size) {
  (void)size;
  free(ptr);
  return 1;
}

#endif

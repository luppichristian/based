// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/allocator.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include <string.h>

func void* _allocator_alloc(allocator alloc, sz size, callsite site) {
  profile_func_begin;
  if (alloc.alloc_fn == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(alloc.alloc_fn != NULL);
  sz request_size = size > 0 ? size : 1;
  if (alloc.alloc_fn) {
    profile_func_end;
    return alloc.alloc_fn(alloc.user_data, site, request_size);
  }
  profile_func_end;
  return NULL;
}

func void* _allocator_calloc(allocator alloc, sz count, sz size, callsite site) {
  profile_func_begin;
  if (count == 0 || size == 0 || count > SZ_MAX / size) {
    profile_func_end;
    return NULL;
  }
  sz total_size = count * size;
  void* ptr = _allocator_alloc(alloc, total_size, site);
  if (ptr) {
    mem_zero(ptr, total_size);
  }
  profile_func_end;
  return ptr;
}

func void _allocator_dealloc(allocator alloc, void* ptr, callsite site) {
  profile_func_begin;
  if (ptr == NULL) {
    profile_func_end;
    return;
  }
  assert(alloc.dealloc_fn != NULL);
  if (alloc.dealloc_fn) {
    alloc.dealloc_fn(alloc.user_data, site, ptr);
  }
  profile_func_end;
}

func void* _allocator_realloc(allocator alloc, void* ptr, sz new_size, callsite site) {
  profile_func_begin;
  if (new_size == 0) {
    profile_func_end;
    return NULL;
  }
  if (ptr == NULL) {
    profile_func_end;
    return _allocator_alloc(alloc, new_size, site);
  }
  assert(alloc.realloc_fn != NULL);
  if (alloc.realloc_fn) {
    profile_func_end;
    return alloc.realloc_fn(alloc.user_data, site, ptr, new_size);
  }
  profile_func_end;
  return NULL;
}
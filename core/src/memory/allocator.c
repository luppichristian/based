// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/allocator.h"
#include "basic/assert.h"
#include "basic/profiler.h"
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
    memset(ptr, 0, total_size);
  }
  profile_func_end;
  return ptr;
}

func void _allocator_dealloc(allocator alloc, void* ptr, sz size, callsite site) {
  profile_func_begin;
  (void)size;
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

func void* _allocator_realloc(allocator alloc, void* ptr, sz old_size, sz new_size, callsite site) {
  profile_func_begin;
  if (new_size == 0) {
    profile_func_end;
    return NULL;
  }
  if (alloc.realloc_fn) {
    profile_func_end;
    return alloc.realloc_fn(alloc.user_data, site, ptr, old_size, new_size);
  }
  void* new_ptr = _allocator_alloc(alloc, new_size, site);
  if (new_ptr && ptr) {
    sz copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    _allocator_dealloc(alloc, ptr, old_size, site);
  }
  profile_func_end;
  return new_ptr;
}

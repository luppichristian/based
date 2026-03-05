// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/allocator.h"
#include "basic/assert.h"
#include <string.h>

func void* _allocator_alloc(allocator* alloc, sz size, callsite site) {
  if (alloc == NULL || alloc->alloc_fn == NULL || size == 0) {
    return NULL;
  }
  assert(alloc->alloc_fn != NULL);
  assert(size > 0);
  if (alloc->alloc_fn) {
    return alloc->alloc_fn(alloc->user_data, site, size);
  }
  return NULL;
}

func void* _allocator_calloc(allocator* alloc, sz count, sz size, callsite site) {
  if (alloc == NULL || count == 0 || size == 0 || count > SZ_MAX / size) {
    return NULL;
  }
  sz total_size = count * size;
  void* ptr = _allocator_alloc(alloc, total_size, site);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  return ptr;
}

func void _allocator_dealloc(allocator* alloc, void* ptr, sz size, callsite site) {
  if (alloc == NULL || ptr == NULL) {
    return;
  }
  assert(alloc->dealloc_fn != NULL);
  if (alloc->dealloc_fn) {
    alloc->dealloc_fn(alloc->user_data, site, ptr);
  }
}

func void* _allocator_realloc(allocator* alloc, void* ptr, sz old_size, sz new_size, callsite site) {
  if (alloc == NULL || new_size == 0) {
    return NULL;
  }
  assert(alloc != NULL);
  if (alloc->realloc_fn) {
    return alloc->realloc_fn(alloc->user_data, site, ptr, old_size, new_size);
  }
  void* new_ptr = _allocator_alloc(alloc, new_size, site);
  if (new_ptr && ptr) {
    sz copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    _allocator_dealloc(alloc, ptr, old_size, site);
  }
  return new_ptr;
}

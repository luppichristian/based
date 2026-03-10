// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/arena.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include <string.h>

// =========================================================================
// Internal Helpers
// =========================================================================

func void arena_block_setup(arena_block* blk, sz size, b8 owned) {
  profile_func_begin;
  blk->next = NULL;
  blk->size = size;
  blk->used = size_of(arena_block);
  blk->owned = owned;
  profile_func_end;
}

func void arena_chain_block(arena* arn, arena_block* blk) {
  profile_func_begin;
  if (arn->blocks_tail) {
    arn->blocks_tail->next = blk;
    arn->blocks_tail = blk;
  } else {
    arn->blocks_head = blk;
    arn->blocks_tail = blk;
  }
  profile_func_end;
}

// Attempts to bump-allocate size bytes aligned to align from blk.
// Returns the aligned pointer on success, NULL if the block has no room.
func void* arena_block_alloc(arena_block* blk, sz size, sz align) {
  profile_func_begin;
  u8* base = (u8*)blk + blk->used;
  sz pad = (sz)(align_up((up)base, align) - (up)base);
  u8* aligned = base + pad;
  sz avail = blk->size - blk->used;
  if (pad > avail || size > avail - pad) {
    profile_func_end;
    return NULL;
  }
  blk->used += pad + size;
  profile_func_end;
  return aligned;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* arena_alloc_callback(void* user_data, callsite site, sz size) {
  profile_func_begin;
  arena* arn = (arena*)user_data;
  profile_func_end;
  return _arena_alloc(arn, size, size_of(void*), site);
}

func void arena_dealloc_callback(void* user_data, callsite site, void* ptr) {
  profile_func_begin;
  (void)user_data;
  (void)site;
  (void)ptr;
  profile_func_end;
}

func void* arena_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  profile_func_begin;
  arena* arn = (arena*)user_data;
  profile_func_end;
  return _arena_realloc(arn, ptr, old_size, new_size, size_of(void*), site);
}

// =========================================================================
// Create / Destroy
// =========================================================================

func arena arena_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz) {
  profile_func_begin;
  arena arn;
  memset(&arn, 0, size_of(arn));
  arn.parent = parent_alloc;
  if (arn.parent.alloc_fn == NULL || arn.parent.dealloc_fn == NULL) {
    arn.parent = thread_get_allocator();
  }
  if (arn.parent.alloc_fn == NULL || arn.parent.dealloc_fn == NULL) {
    arn.parent = global_get_allocator();
  }
  arn.opt_mutex = opt_mutex;
  arn.default_block_sz = default_block_sz;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_ARENA,
                                                     .object_ptr = &arn,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("arena_create: block_sz=%zu", (size_t)default_block_sz);
  profile_func_end;
  return arn;
}

func arena arena_create_mutexed(allocator parent_alloc, sz default_block_sz) {
  profile_func_begin;
  arena arn = arena_create(parent_alloc, mutex_create(), default_block_sz);
  arn.mutex_owned = 1;
  profile_func_end;
  return arn;
}

func void arena_destroy(arena* arn) {
  profile_func_begin;
  if (arn == NULL) {
    profile_func_end;
    return;
  }
  assert(arn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_ARENA,
                                                     .object_ptr = arn,
                                                 });
  (void)msg_post(&lifecycle_msg);

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = arn->blocks_head;
  while (blk) {
    arena_block* nxt = blk->next;
    if (blk->owned && arn->parent.alloc_fn) {
      _allocator_dealloc(arn->parent, blk, blk->size, CALLSITE_HERE);
    }
    blk = nxt;
  }

  arn->blocks_head = NULL;
  arn->blocks_tail = NULL;

  mutex mtx_owned = arn->mutex_owned ? arn->opt_mutex : NULL;

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  if (mtx_owned) {
    mutex_destroy(mtx_owned);
  }

  arn->opt_mutex = NULL;
  arn->mutex_owned = 0;
  thread_log_trace("arena_destroy: arn=%p", (void*)arn);
  profile_func_end;
}

func allocator arena_get_allocator(arena* arn) {
  allocator alloc;
  alloc.user_data = arn;
  alloc.alloc_fn = arena_alloc_callback;
  alloc.dealloc_fn = arena_dealloc_callback;
  alloc.realloc_fn = arena_realloc_callback;
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void arena_add_block(arena* arn, void* ptr, sz size) {
  profile_func_begin;
  if (arn == NULL || ptr == NULL || size <= size_of(arena_block)) {
    profile_func_end;
    return;
  }
  assert(arn != NULL);
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = (arena_block*)ptr;
  arena_block_setup(blk, size, 0);
  arena_chain_block(arn, blk);

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  profile_func_end;
}

func b32 arena_remove_block(arena* arn, void* ptr) {
  profile_func_begin;
  if (arn == NULL || ptr == NULL) {
    profile_func_end;
    return false;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  b32 found = false;
  arena_block* prev = NULL;
  arena_block* blk = arn->blocks_head;

  while (blk) {
    if ((void*)blk == ptr) {
      if (prev) {
        prev->next = blk->next;
      } else {
        arn->blocks_head = blk->next;
      }
      if (arn->blocks_tail == blk) {
        arn->blocks_tail = prev;
      }
      found = 1;
      break;
    }
    prev = blk;
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  profile_func_end;
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _arena_alloc(arena* arn, sz size, sz align, callsite site) {
  profile_func_begin;
  if (arn == NULL || size == 0 || align == 0) {
    profile_func_end;
    return NULL;
  }
  assert(align > 0);
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  void* result = NULL;

  arena_block* blk = arn->blocks_head;
  while (blk && !result) {
    result = arena_block_alloc(blk, size, align);
    blk = blk->next;
  }

  if (!result && arn->parent.alloc_fn) {
    sz needed = size_of(arena_block) + align + size;
    sz block_sz = arn->default_block_sz > needed ? arn->default_block_sz : needed;
    arena_block* new_blk = (arena_block*)_allocator_alloc(arn->parent, block_sz, site);
    if (new_blk) {
      arena_block_setup(new_blk, block_sz, 1);
      arena_chain_block(arn, new_blk);
      result = arena_block_alloc(new_blk, size, align);
    }
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  profile_func_end;
  return result;
}

func void* _arena_realloc(
    arena* arn,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site) {
  profile_func_begin;
  if (!ptr) {
    profile_func_end;
    return _arena_alloc(arn, new_size, align, site);
  }

  b32 done = false;
  void* result = ptr;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  // Try to extend the last allocation in-place by moving the cursor.
  arena_block* blk = arn->blocks_head;
  while (blk && !done) {
    u8* cursor = (u8*)blk + blk->used;
    if ((u8*)ptr + old_size == cursor) {
      if (new_size <= old_size) {
        blk->used -= old_size - new_size;
        done = 1;
      } else {
        sz extra = new_size - old_size;
        sz avail = blk->size - blk->used;
        if (extra <= avail) {
          blk->used += extra;
          done = 1;
        }
      }
    }
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  if (!done) {
    result = _arena_alloc(arn, new_size, align, site);
    if (result) {
      sz copy_sz = old_size < new_size ? old_size : new_size;
      memcpy(result, ptr, copy_sz);
    }
  }

  profile_func_end;
  return result;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void arena_clear(arena* arn) {
  profile_func_begin;
  if (arn == NULL) {
    profile_func_end;
    return;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = arn->blocks_head;
  while (blk) {
    blk->used = size_of(arena_block);
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  profile_func_end;
}

func sz arena_block_count(arena* arn) {
  if (arn == NULL) {
      return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz count = 0;
  for (arena_block* blk = arn->blocks_head; blk != NULL; blk = blk->next) {
    count += 1;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return count;
}

func sz arena_total_size(arena* arn) {
  if (arn == NULL) {
      return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  for (arena_block* blk = arn->blocks_head; blk != NULL; blk = blk->next) {
    total += blk->size;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}

func sz arena_total_used(arena* arn) {
  if (arn == NULL) {
      return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  for (arena_block* blk = arn->blocks_head; blk != NULL; blk = blk->next) {
    total += blk->used > size_of(arena_block) ? blk->used - size_of(arena_block) : 0;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}

func sz arena_total_free(arena* arn) {
  if (arn == NULL) {
      return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  for (arena_block* blk = arn->blocks_head; blk != NULL; blk = blk->next) {
    if (blk->size > blk->used) {
      total += blk->size - blk->used;
    }
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}

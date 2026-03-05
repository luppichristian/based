// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/global_ctx.h"
#include "input/msg.h"
#include "memory/vmem.h"
#include "threads/atomics.h"

#include <string.h>

global_var global_ctx process_global_ctx = {0};
global_var atomic_i32 process_global_ctx_init = {0};

func b32 global_ctx_init(allocator main_allocator) {
  if (!main_allocator.alloc_fn) {
    return false;
  }

  i32 state = atomic_i32_get(&process_global_ctx_init);
  if (state == 2) {
    return true;
  }

  i32 expected = 0;
  if (atomic_i32_cmpex(&process_global_ctx_init, &expected, 1)) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_GLOBAL_CTX;
    lifecycle_msg.object_lifecycle.object_ptr = &process_global_ctx;
    if (!msg_post(&lifecycle_msg)) {
      atomic_fence_release();
      atomic_i32_set(&process_global_ctx_init, 0);
      return false;
    }

    memset(&process_global_ctx, 0, sizeof(process_global_ctx));
    process_global_ctx.mutex_handle = mutex_create();
    if (!process_global_ctx.mutex_handle || !ctx_init(&process_global_ctx.shared_ctx, main_allocator, true)) {
      if (process_global_ctx.mutex_handle) {
        mutex_destroy(process_global_ctx.mutex_handle);
      }
      memset(&process_global_ctx, 0, sizeof(process_global_ctx));
      atomic_fence_release();
      atomic_i32_set(&process_global_ctx_init, 0);
      return false;
    }

    process_global_ctx.is_init = true;
    atomic_fence_release();
    atomic_i32_set(&process_global_ctx_init, 2);
    return true;
  }

  while (atomic_i32_get(&process_global_ctx_init) == 1) {
    atomic_pause();
  }
  atomic_fence_acquire();
  return atomic_i32_get(&process_global_ctx_init) == 2;
}

func void global_ctx_quit(void) {
  i32 expected = 2;
  if (!atomic_i32_cmpex(&process_global_ctx_init, &expected, 1)) {
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_GLOBAL_CTX;
  lifecycle_msg.object_lifecycle.object_ptr = &process_global_ctx;
  if (!msg_post(&lifecycle_msg)) {
    atomic_fence_release();
    atomic_i32_set(&process_global_ctx_init, 2);
    return;
  }

  mutex wrapper_mutex = process_global_ctx.mutex_handle;
  if (wrapper_mutex) {
    mutex_lock(wrapper_mutex);
  }

  ctx_quit(&process_global_ctx.shared_ctx);
  process_global_ctx.is_init = false;

  if (wrapper_mutex) {
    mutex_unlock(wrapper_mutex);
    mutex_destroy(wrapper_mutex);
  }

  memset(&process_global_ctx, 0, sizeof(process_global_ctx));
  atomic_fence_release();
  atomic_i32_set(&process_global_ctx_init, 0);
}

func b32 global_ctx_is_init(void) {
  return atomic_i32_get(&process_global_ctx_init) == 2;
}

func global_ctx* global_ctx_get(void) {
  if (!global_ctx_is_init() && !global_ctx_init(vmem_get_allocator())) {
    return NULL;
  }
  return &process_global_ctx;
}

func ctx* global_ctx_get_shared(void) {
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper) {
    return NULL;
  }
  return &wrapper->shared_ctx;
}

func log_state* global_get_log_state(void) {
  return ctx_get_log_state(global_ctx_get_shared());
}

func void global_ctx_lock(void) {
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }
}

func void global_ctx_unlock(void) {
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }
}

func allocator global_get_allocator(void) {
  return ctx_get_allocator(global_ctx_get_shared());
}

func arena* global_get_perm_arena(void) {
  return ctx_get_perm_arena(global_ctx_get_shared());
}

func arena* global_get_temp_arena(void) {
  return ctx_get_temp_arena(global_ctx_get_shared());
}

func heap* global_get_perm_heap(void) {
  return ctx_get_perm_heap(global_ctx_get_shared());
}

func heap* global_get_temp_heap(void) {
  return ctx_get_temp_heap(global_ctx_get_shared());
}

func void* global_get_user_data(sz index) {
  return ctx_get_user_data(global_ctx_get_shared(), index);
}

func b32 global_set_user_data(sz index, void* user_data) {
  return ctx_set_user_data(global_ctx_get_shared(), index, user_data);
}

func void global_clear_temp(void) {
  ctx_clear_temp(global_ctx_get_shared());
}

func void global_log_set_level(log_level level) {
  log_state_set_level(global_get_log_state(), level);
}

func void global_log_begin_frame(void) {
  log_state_begin_frame(global_get_log_state());
}

func log_frame* global_log_end_frame(u32 severity_mask) {
  return log_state_end_frame(global_get_log_state(), severity_mask);
}

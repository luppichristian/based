// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// SDL_ThreadFunction is `int (SDLCALL *)(void*)`.
// SDLCALL is __cdecl on Windows, which is the default C calling convention,
// so casting thread_func (i32 (*)(void*)) to SDL_ThreadFunction is safe on all targets.

typedef struct thread_entry_payload {
  thread_func entry;
  void* arg;
  allocator payload_allocator;
  allocator main_allocator;
  b32 should_init_thread_ctx;
} thread_entry_payload;

func allocator thread_allocator_resolve(allocator preferred_alloc) {
  profile_func_begin;
  if (preferred_alloc.alloc_fn != NULL && preferred_alloc.dealloc_fn != NULL) {
    profile_func_end;
    return preferred_alloc;
  }

  allocator thread_alloc = thread_get_allocator();
  if (thread_alloc.alloc_fn != NULL && thread_alloc.dealloc_fn != NULL) {
    profile_func_end;
    return thread_alloc;
  }

  profile_func_end;
  return global_get_allocator();
}

func i32 thread_entry_wrapper(void* raw) {
  profile_func_begin;
  thread_entry_payload* payload = (thread_entry_payload*)raw;
  b32 has_thread_ctx = false;
  i32 exit_code = 0;

  if (payload == NULL || payload->entry == NULL) {
    thread_log_error("Rejected thread entry wrapper payload=%p has_entry=%u",
                     raw,
                     (u32)(payload != NULL && payload->entry != NULL));
    profile_func_end;
    return 0;
  }
  assert(payload->payload_allocator.dealloc_fn != NULL);

  if (payload->should_init_thread_ctx) {
    has_thread_ctx = thread_ctx_init(payload->main_allocator);
    if (!has_thread_ctx) {
      thread_log_warn("Thread entry running without thread context");
    } else {
      thread_log_trace("Initialized thread context for worker thread");
    }
  }

  thread_log_trace("Entering worker thread payload=%p", raw);
  exit_code = payload->entry(payload->arg);
  thread_log_trace("Leaving worker thread exit_code=%d", exit_code);

  if (has_thread_ctx) {
    thread_ctx_quit();
  }

  allocator_dealloc(payload->payload_allocator, payload, size_of(*payload));
  profile_func_end;
  return exit_code;
}

func thread thread_create_impl(thread_func entry, void* arg, cstr8 name, allocator main_allocator) {
  profile_func_begin;
  allocator payload_allocator = {0};
  if (!entry) {
    thread_log_error("Rejected thread creation without entry callback");
    profile_func_end;
    return NULL;
  }
  payload_allocator = thread_allocator_resolve(main_allocator);
  assert(payload_allocator.alloc_fn != NULL);
  assert(payload_allocator.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = NULL,
                                                 });
  (void)msg_post(&lifecycle_msg);

  thread_entry_payload* payload = (thread_entry_payload*)allocator_alloc(payload_allocator, size_of(*payload));
  if (!payload) {
    thread_log_error("Failed to allocate thread payload name=%s", name != NULL ? name : "<null>");
    profile_func_end;
    return NULL;
  }

  SDL_zero(*payload);
  payload->entry = entry;
  payload->arg = arg;
  payload->payload_allocator = payload_allocator;
  if (main_allocator.alloc_fn) {
    payload->main_allocator = main_allocator;
    payload->should_init_thread_ctx = true;
  }

  thread thd = (thread)SDL_CreateThread(thread_entry_wrapper, name, payload);
  if (!thd) {
    allocator_dealloc(payload_allocator, payload, size_of(*payload));
    thread_log_error("Failed to create thread name=%s error=%s", name != NULL ? name : "<null>", SDL_GetError());
    profile_func_end;
    return NULL;
  }
  thread_log_trace("Created thread handle=%p name=%s", thd, name != NULL ? name : "<null>");
  profile_func_end;
  return thd;
}

func thread _thread_create(thread_func entry, void* arg, allocator main_allocator, callsite site) {
  profile_func_begin;
  (void)site;
  profile_func_end;
  return thread_create_impl(entry, arg, NULL, main_allocator);
}

func thread _thread_create_named(
    thread_func entry,
    void* arg,
    cstr8 name,
    allocator main_allocator,
    callsite site) {
  profile_func_begin;
  (void)site;
  profile_func_end;
  return thread_create_impl(entry, arg, name, main_allocator);
}

func b32 thread_is_valid(thread thd) {
  return thd != NULL;
}

func b32 thread_join(thread thd, i32* out_exit_code) {
  profile_func_begin;
  if (!thd) {
    thread_log_error("Cannot join invalid thread");
    profile_func_end;
    return false;
  }
  assert(thread_is_valid(thd));
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = thd,
                                                 });
  (void)msg_post(&lifecycle_msg);
  SDL_WaitThread((SDL_Thread*)thd, (int*)out_exit_code);
  thread_log_trace("Joined thread handle=%p", thd);
  profile_func_end;
  return true;
}

func void thread_detach(thread thd) {
  profile_func_begin;
  if (!thd) {
    thread_log_warn("Skipping detach for invalid thread");
    profile_func_end;
    return;
  }
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = thd,
                                                 });
  (void)msg_post(&lifecycle_msg);
  SDL_DetachThread((SDL_Thread*)thd);
  thread_log_trace("Detached thread handle=%p", thd);
  profile_func_end;
}

func u64 thread_get_id(thread thd) {
  return (u64)SDL_GetThreadID((SDL_Thread*)thd);
}

func cstr8 thread_get_name(thread thd) {
  return SDL_GetThreadName((SDL_Thread*)thd);
}

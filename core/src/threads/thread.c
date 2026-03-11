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
  ctx_setup setup;
} thread_entry_payload;

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
    return 1;
  }

  heap* hp = global_get_perm_heap();
  if (!hp) {
    global_log_error("Failed to acquire global heap");
    profile_func_end;
    return 1;
  }

  if (payload->setup.main_allocator.alloc_fn) {
    if (!thread_ctx_init(payload->setup)) {
      thread_log_error("Could not initialize thread ctx");
      heap_dealloc(hp, payload);
      profile_func_end;
      return 1;
    }
    has_thread_ctx = true;
  }

  thread_log_trace("Entering worker thread payload=%p", raw);
  exit_code = payload->entry(payload->arg);
  thread_log_trace("Leaving worker thread exit_code=%d", exit_code);

  if (has_thread_ctx) {
    if (!thread_ctx_quit()) {
      thread_log_error("Failed to quit thread context");
    }
  }

  heap_dealloc(hp, payload);
  profile_func_end;
  return exit_code;
}

func thread thread_create_impl(
    thread_func entry,
    void* arg,
    cstr8 name,
    ctx_setup setup,
    callsite site) {
  profile_func_begin;

  if (!entry) {
    thread_log_error("Rejected thread creation without entry callback");
    profile_func_end;
    return NULL;
  }

  heap* hp = global_get_perm_heap();
  if (!hp) {
    global_log_error("Failed to acquire global heap");
    profile_func_end;
    return NULL;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
      .object_ptr = NULL,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Thread creation cancelled");
    profile_func_end;
    return NULL;
  }

  thread_entry_payload* payload = heap_alloc_type(hp, thread_entry_payload);
  if (!payload) {
    thread_log_error("Failed to allocate thread payload name=%s", name != NULL ? name : "<null>");
    profile_func_end;
    return NULL;
  }

  SDL_zero(*payload);
  payload->entry = entry;
  payload->arg = arg;
  payload->setup = setup;

  thread thd = (thread)SDL_CreateThread(thread_entry_wrapper, name, payload);
  if (!thd) {
    heap_dealloc(hp, payload);
    thread_log_error("Failed to create thread name=%s error=%s", name != NULL ? name : "<null>", SDL_GetError());
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created thread handle=%p name=%s", thd, name != NULL ? name : "<null>");
  profile_func_end;
  return thd;
}

func thread _thread_create(
    thread_func entry,
    void* arg,
    ctx_setup setup,
    callsite site) {
  return thread_create_impl(entry, arg, NULL, setup, site);
}

func thread _thread_create_named(
    thread_func entry,
    void* arg,
    cstr8 name,
    ctx_setup setup,
    callsite site) {
  return thread_create_impl(entry, arg, name, setup, site);
}

func b32 thread_is_valid(thread thd) {
  return thd != NULL;
}

func b32 _thread_join(thread thd, i32* out_exit_code, callsite site) {
  profile_func_begin;
  if (!thread_is_valid(thd)) {
    thread_log_error("Cannot join invalid thread");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
      .object_ptr = thd,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Thread join cancelled");
    profile_func_end;
    return false;
  }

  SDL_WaitThread((SDL_Thread*)thd, (int*)out_exit_code);
  thread_log_trace("Joined thread handle=%p", thd);
  profile_func_end;
  return true;
}

func b32 _thread_detach(thread thd, callsite site) {
  profile_func_begin;
  if (!thd) {
    thread_log_warn("Skipping detach for invalid thread");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
      .object_ptr = thd,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Thread detach cancelled");
    profile_func_end;
    return false;
  }

  (void)msg_post(&lifecycle_msg);
  SDL_DetachThread((SDL_Thread*)thd);
  thread_log_trace("Detached thread handle=%p", thd);
  profile_func_end;
  return true;
}

func u64 thread_get_id(thread thd) {
  return (u64)SDL_GetThreadID((SDL_Thread*)thd);
}

func cstr8 thread_get_name(thread thd) {
  return SDL_GetThreadName((SDL_Thread*)thd);
}

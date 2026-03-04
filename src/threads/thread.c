// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread.h"
#include "threads/thread_ctx.h"
#include "../sdl3_include.h"

// SDL_ThreadFunction is `int (SDLCALL *)(void*)`.
// SDLCALL is __cdecl on Windows, which is the default C calling convention,
// so casting thread_func (i32 (*)(void*)) to SDL_ThreadFunction is safe on all targets.

typedef struct thread_entry_payload {
  thread_func entry;
  void* arg;
  allocator main_allocator;
  b32 should_init_thread_ctx;
} thread_entry_payload;

func i32 thread_entry_wrapper(void* raw) {
  thread_entry_payload* payload = (thread_entry_payload*)raw;
  b32 has_thread_ctx = false;

  if (payload->should_init_thread_ctx) {
    has_thread_ctx = thread_ctx_init(payload->main_allocator);
  }

  i32 exit_code = payload->entry(payload->arg);

  if (has_thread_ctx) {
    thread_ctx_quit();
  }

  SDL_free(payload);
  return exit_code;
}

func thread thread_create_impl(thread_func entry, void* arg, const c8* name, allocator main_allocator) {
  if (!entry) {
    return NULL;
  }

  thread_entry_payload* payload = (thread_entry_payload*)SDL_malloc(sizeof(*payload));
  if (!payload) {
    return NULL;
  }

  SDL_zero(*payload);
  payload->entry = entry;
  payload->arg = arg;
  if (main_allocator.alloc_fn) {
    payload->main_allocator = main_allocator;
    payload->should_init_thread_ctx = true;
  }

  thread thd = (thread)SDL_CreateThread(thread_entry_wrapper, name, payload);
  if (!thd) {
    SDL_free(payload);
  }
  return thd;
}

func thread _thread_create(thread_func entry, void* arg, allocator main_allocator, callsite site) {
  (void)site;
  return thread_create_impl(entry, arg, NULL, main_allocator);
}

func thread _thread_create_named(
    thread_func entry,
    void* arg,
    const c8* name,
    allocator main_allocator,
    callsite site) {
  (void)site;
  return thread_create_impl(entry, arg, name, main_allocator);
}

func b32 thread_is_valid(thread thd) {
  return thd != NULL;
}

func b32 thread_join(thread thd, i32* out_exit_code) {
  if (!thd) return 0;
  SDL_WaitThread((SDL_Thread*)thd, (int*)out_exit_code);
  return 1;
}

func void thread_detach(thread thd) {
  SDL_DetachThread((SDL_Thread*)thd);
}

func u64 thread_get_id(thread thd) {
  return (u64)SDL_GetThreadID((SDL_Thread*)thd);
}

func const c8* thread_get_name(thread thd) {
  return SDL_GetThreadName((SDL_Thread*)thd);
}

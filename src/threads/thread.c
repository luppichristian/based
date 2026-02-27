// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread.h"
#include "../sdl3_include.h"

// SDL_ThreadFunction is `int (SDLCALL *)(void*)`.
// SDLCALL is __cdecl on Windows, which is the default C calling convention,
// so casting thread_func (i32 (*)(void*)) to SDL_ThreadFunction is safe on all targets.

func thread _thread_create(thread_func entry, void* arg, callsite site) {
  (void)site;
  return (thread)SDL_CreateThread((SDL_ThreadFunction)entry, NULL, arg);
}

func thread _thread_create_named(thread_func entry, void* arg, const c8* name, callsite site) {
  (void)site;
  return (thread)SDL_CreateThread((SDL_ThreadFunction)entry, name, arg);
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
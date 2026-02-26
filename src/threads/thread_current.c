// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_current.h"
#include <SDL3/SDL.h>

func u64 thread_id(void) {
  return (u64)SDL_GetCurrentThreadID();
}

func b32 thread_set_priority(thread_priority priority) {
  static const SDL_ThreadPriority sdl_priorities[] = {
      SDL_THREAD_PRIORITY_LOW,
      SDL_THREAD_PRIORITY_NORMAL,
      SDL_THREAD_PRIORITY_HIGH,
      SDL_THREAD_PRIORITY_TIME_CRITICAL,
  };
  return SDL_SetCurrentThreadPriority(sdl_priorities[priority]);
}

func void thread_sleep(u32 millis) {
  SDL_Delay((Uint32)millis);
}

func void thread_yield(void) {
  SDL_Delay(0);
}

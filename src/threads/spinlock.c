// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include <SDL3/SDL.h>

// spinlock { int state; } is layout-compatible with SDL_SpinLock (typedef int SDL_SpinLock).
SDL_COMPILE_TIME_ASSERT(spinlock_compat, sizeof(spinlock) == sizeof(SDL_SpinLock));

func void spinlock_lock(spinlock* sl) {
  SDL_LockSpinlock((SDL_SpinLock*)&sl->state);
}

func void spinlock_unlock(spinlock* sl) {
  SDL_UnlockSpinlock((SDL_SpinLock*)&sl->state);
}

func b32 spinlock_trylock(spinlock* sl) {
  return SDL_TryLockSpinlock((SDL_SpinLock*)&sl->state);
}

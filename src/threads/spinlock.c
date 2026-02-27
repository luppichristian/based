// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include <SDL3/SDL.h>

func spinlock spinlock_create() {
  SDL_SpinLock* sl = (SDL_SpinLock*)SDL_malloc(sizeof(SDL_SpinLock));
  if (sl) {
    *sl = 0;
  }
  return (spinlock)sl;
}

func void spinlock_destroy(spinlock sl) {
  SDL_free(sl);
}

func b32 spinlock_is_valid(spinlock sl) {
  return sl != NULL;
}

func void spinlock_lock(spinlock sl) {
  SDL_LockSpinlock((SDL_SpinLock*)sl);
}

func void spinlock_unlock(spinlock sl) {
  SDL_UnlockSpinlock((SDL_SpinLock*)sl);
}

func b32 spinlock_trylock(spinlock sl) {
  return SDL_TryLockSpinlock((SDL_SpinLock*)sl);
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/mutex.h"
#include <SDL3/SDL.h>

func mutex mutex_create(void) {
  return (mutex)SDL_CreateMutex();
}

func b32 mutex_destroy(mutex mtx) {
  if (!mtx) {
    return 0;
  }
  SDL_DestroyMutex((SDL_Mutex*)mtx);
  return 1;
}

func b32 mutex_is_valid(mutex mtx) {
  return mtx != NULL;
}

func void mutex_lock(mutex mtx) {
  SDL_LockMutex((SDL_Mutex*)mtx);
}

func b32 mutex_trylock(mutex mtx) {
  return SDL_TryLockMutex((SDL_Mutex*)mtx);
}

func void mutex_unlock(mutex mtx) {
  SDL_UnlockMutex((SDL_Mutex*)mtx);
}

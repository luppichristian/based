// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/semaphore.h"
#include <SDL3/SDL.h>

func semaphore semaphore_create(u32 initial_count) {
  return (semaphore)SDL_CreateSemaphore((Uint32)initial_count);
}

func b32 semaphore_destroy(semaphore sem) {
  if (!sem) {
    return 0;
  }
  SDL_DestroySemaphore((SDL_Semaphore*)sem);
  return 1;
}

func b32 semaphore_is_valid(semaphore sem) {
  return sem != NULL;
}

func void semaphore_wait(semaphore sem) {
  SDL_WaitSemaphore((SDL_Semaphore*)sem);
}

func void semaphore_signal(semaphore sem) {
  SDL_SignalSemaphore((SDL_Semaphore*)sem);
}

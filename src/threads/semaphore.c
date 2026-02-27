// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/semaphore.h"
#include "../sdl3_include.h"

func semaphore _semaphore_create(u32 initial_count, callsite site) {
  (void)site;
  return (semaphore)SDL_CreateSemaphore((Uint32)initial_count);
}

func b32 _semaphore_destroy(semaphore sem, callsite site) {
  (void)site;
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

func b32 semaphore_trywait(semaphore sem) {
  return SDL_TryWaitSemaphore((SDL_Semaphore*)sem);
}

func b32 semaphore_wait_timeout(semaphore sem, u32 millis) {
  return SDL_WaitSemaphoreTimeout((SDL_Semaphore*)sem, (Sint32)millis);
}

func void semaphore_signal(semaphore sem) {
  SDL_SignalSemaphore((SDL_Semaphore*)sem);
}

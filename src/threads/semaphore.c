// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/semaphore.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func semaphore _semaphore_create(u32 initial_count, callsite site) {
  (void)site;
  semaphore handle = (semaphore)SDL_CreateSemaphore((Uint32)initial_count);
  if (handle != NULL && !msg_post_object_event(MSG_OBJECT_EVENT_CREATE, MSG_OBJECT_TYPE_SEMAPHORE, handle)) {
    SDL_DestroySemaphore((SDL_Semaphore*)handle);
    return NULL;
  }
  return handle;
}

func b32 _semaphore_destroy(semaphore sem, callsite site) {
  (void)site;
  if (!sem) {
    return 0;
  }

  if (!msg_post_object_event(MSG_OBJECT_EVENT_DESTROY, MSG_OBJECT_TYPE_SEMAPHORE, sem)) {
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

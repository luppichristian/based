// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/condvar.h"
#include <SDL3/SDL.h>

func condvar condvar_create(void) {
  return (condvar)SDL_CreateCondition();
}

func b32 condvar_destroy(condvar cond) {
  if (!cond) {
    return 0;
  }
  SDL_DestroyCondition((SDL_Condition*)cond);
  return 1;
}

func b32 condvar_is_valid(condvar cond) {
  return cond != NULL;
}

func void condvar_wait(condvar cond, mutex mtx) {
  SDL_WaitCondition((SDL_Condition*)cond, (SDL_Mutex*)mtx);
}

func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis) {
  return SDL_WaitConditionTimeout((SDL_Condition*)cond, (SDL_Mutex*)mtx, (Sint32)millis);
}

func void condvar_signal(condvar cond) {
  SDL_SignalCondition((SDL_Condition*)cond);
}

func void condvar_broadcast(condvar cond) {
  SDL_BroadcastCondition((SDL_Condition*)cond);
}

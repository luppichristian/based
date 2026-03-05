// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/condvar.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func condvar _condvar_create(callsite site) {
  (void)site;
  condvar handle = (condvar)SDL_CreateCondition();
  if (handle != NULL && !msg_post_object_event(MSG_OBJECT_EVENT_CREATE, MSG_OBJECT_TYPE_CONDVAR, handle)) {
    SDL_DestroyCondition((SDL_Condition*)handle);
    return NULL;
  }
  return handle;
}

func b32 _condvar_destroy(condvar cond, callsite site) {
  (void)site;
  if (!cond) {
    return 0;
  }

  if (!msg_post_object_event(MSG_OBJECT_EVENT_DESTROY, MSG_OBJECT_TYPE_CONDVAR, cond)) {
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

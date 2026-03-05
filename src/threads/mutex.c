// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/mutex.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func mutex _mutex_create(callsite site) {
  (void)site;
  mutex handle = (mutex)SDL_CreateMutex();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_MUTEX;
    lifecycle_msg.object_lifecycle.object_ptr = handle;
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyMutex((SDL_Mutex*)handle);
      return NULL;
    }
  }
  return handle;
}

func b32 _mutex_destroy(mutex mtx, callsite site) {
  (void)site;
  if (!mtx) {
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_MUTEX;
  lifecycle_msg.object_lifecycle.object_ptr = mtx;
  if (!msg_post(&lifecycle_msg)) {
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

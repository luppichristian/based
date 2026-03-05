// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func spinlock _spinlock_create(callsite site) {
  (void)site;
  SDL_SpinLock* spl = (SDL_SpinLock*)SDL_malloc(sizeof(SDL_SpinLock));
  if (spl) {
    *spl = 0;
  }
  if (spl != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_SPINLOCK;
    lifecycle_msg.object_lifecycle.object_ptr = spl;
    if (!msg_post(&lifecycle_msg)) {
      SDL_free(spl);
      return NULL;
    }
  }
  return (spinlock)spl;
}

func void _spinlock_destroy(spinlock sl, callsite site) {
  (void)site;
  if (!sl) {
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_SPINLOCK;
  lifecycle_msg.object_lifecycle.object_ptr = sl;
  if (!msg_post(&lifecycle_msg)) {
    return;
  }
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

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/rwlock.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func rwlock _rwlock_create(callsite site) {
  (void)site;
  rwlock handle = (rwlock)SDL_CreateRWLock();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_RWLOCK;
    lifecycle_msg.object_lifecycle.object_ptr = handle;
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyRWLock((SDL_RWLock*)handle);
      return NULL;
    }
  }
  return handle;
}

func b32 _rwlock_destroy(rwlock rw, callsite site) {
  (void)site;
  if (!rw) {
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_RWLOCK;
  lifecycle_msg.object_lifecycle.object_ptr = rw;
  if (!msg_post(&lifecycle_msg)) {
    return 0;
  }
  SDL_DestroyRWLock((SDL_RWLock*)rw);
  return 1;
}

func b32 rwlock_is_valid(rwlock rw) {
  return rw != NULL;
}

func void rwlock_read_lock(rwlock rw) {
  SDL_LockRWLockForReading((SDL_RWLock*)rw);
}

func void rwlock_read_unlock(rwlock rw) {
  SDL_UnlockRWLock((SDL_RWLock*)rw);
}

func void rwlock_write_lock(rwlock rw) {
  SDL_LockRWLockForWriting((SDL_RWLock*)rw);
}

func void rwlock_write_unlock(rwlock rw) {
  SDL_UnlockRWLock((SDL_RWLock*)rw);
}

func b32 rwlock_try_read_lock(rwlock rw) {
  return SDL_TryLockRWLockForReading((SDL_RWLock*)rw);
}

func b32 rwlock_try_write_lock(rwlock rw) {
  return SDL_TryLockRWLockForWriting((SDL_RWLock*)rw);
}

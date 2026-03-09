// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/rwlock.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func rwlock _rwlock_create(callsite site) {
  profile_func_begin;
  (void)site;
  rwlock handle = (rwlock)SDL_CreateRWLock();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
                                                       .object_ptr = handle,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyRWLock((SDL_RWLock*)handle);
      profile_func_end;
      return NULL;
    }
    thread_log_trace("rwlock_create: handle=%p", handle);
  }
  profile_func_end;
  return handle;
}

func b32 _rwlock_destroy(rwlock rw, callsite site) {
  profile_func_begin;
  (void)site;
  if (!rw) {
    profile_func_end;
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
                                                     .object_ptr = rw,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    profile_func_end;
    return 0;
  }
  thread_log_trace("rwlock_destroy: handle=%p", rw);
  SDL_DestroyRWLock((SDL_RWLock*)rw);
  profile_func_end;
  return 1;
}

func b32 rwlock_is_valid(rwlock rw) {
  profile_func_begin;
  profile_func_end;
  return rw != NULL;
}

func void rwlock_read_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForReading((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_read_unlock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_write_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForWriting((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_write_unlock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  profile_func_end;
}

func b32 rwlock_try_read_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return 0;
  }
  assert(rw != NULL);
  profile_func_end;
  return SDL_TryLockRWLockForReading((SDL_RWLock*)rw);
}

func b32 rwlock_try_write_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    profile_func_end;
    return 0;
  }
  assert(rw != NULL);
  profile_func_end;
  return SDL_TryLockRWLockForWriting((SDL_RWLock*)rw);
}

func b32 rwlock_timed_read_lock(rwlock rw, i32 timeout_ms) {
  profile_func_begin;
  if (rw == NULL || timeout_ms < 0) {
    profile_func_end;
    return 0;
  }

  u64 start_ticks = SDL_GetTicks();
  while (!rwlock_try_read_lock(rw)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      profile_func_end;
      return 0;
    }
    SDL_Delay(1);
  }

  profile_func_end;
  return 1;
}

func b32 rwlock_timed_write_lock(rwlock rw, i32 timeout_ms) {
  profile_func_begin;
  if (rw == NULL || timeout_ms < 0) {
    profile_func_end;
    return 0;
  }

  u64 start_ticks = SDL_GetTicks();
  while (!rwlock_try_write_lock(rw)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      profile_func_end;
      return 0;
    }
    SDL_Delay(1);
  }

  profile_func_end;
  return 1;
}

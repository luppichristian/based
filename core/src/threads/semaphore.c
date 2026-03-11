// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/semaphore.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func semaphore _semaphore_create(u32 initial_count, callsite site) {
  profile_func_begin;
  semaphore handle = (semaphore)SDL_CreateSemaphore((Uint32)initial_count);
  if (handle == NULL) {
    thread_log_error("Failed to create semaphore count=%u error=%s", initial_count, SDL_GetError());
    profile_func_end;
    return NULL;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_SEMAPHORE,
      .object_ptr = handle,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Semaphore creation cancelled");
    SDL_DestroySemaphore((SDL_Semaphore*)handle);
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created semaphore handle=%p count=%u", handle, initial_count);
  profile_func_end;
  return handle;
}

func b32 _semaphore_destroy(semaphore sem, callsite site) {
  profile_func_begin;
  (void)site;
  if (!sem) {
    thread_log_warn("Skipping semaphore destroy for invalid handle");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_SEMAPHORE,
      .object_ptr = sem,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Semaphore destruction cancelled");
    profile_func_end;
    return false;
  }

  thread_log_trace("Destroyed semaphore handle=%p", sem);
  SDL_DestroySemaphore((SDL_Semaphore*)sem);
  profile_func_end;
  return true;
}

func b32 semaphore_is_valid(semaphore sem) {
  return sem != NULL;
}

func void semaphore_wait(semaphore sem) {
  profile_func_begin;
  if (sem == NULL) {
    thread_log_error("Rejected semaphore wait for invalid handle");
    profile_func_end;
    return;
  }
  assert(sem != NULL);
  SDL_WaitSemaphore((SDL_Semaphore*)sem);
  profile_func_end;
}

func b32 semaphore_try_wait(semaphore sem) {
  profile_func_begin;
  if (sem == NULL) {
    thread_log_error("Rejected semaphore try wait for invalid handle");
    profile_func_end;
    return false;
  }
  assert(sem != NULL);
  profile_func_end;
  return SDL_TryWaitSemaphore((SDL_Semaphore*)sem);
}

func b32 semaphore_wait_timeout(semaphore sem, u32 millis) {
  profile_func_begin;
  if (sem == NULL) {
    thread_log_error("Rejected semaphore wait timeout for invalid handle");
    profile_func_end;
    return false;
  }
  assert(sem != NULL);
  profile_func_end;
  return SDL_WaitSemaphoreTimeout((SDL_Semaphore*)sem, (Sint32)millis);
}

func void semaphore_signal(semaphore sem) {
  profile_func_begin;
  if (sem == NULL) {
    thread_log_error("Rejected semaphore signal for invalid handle");
    profile_func_end;
    return;
  }
  assert(sem != NULL);
  SDL_SignalSemaphore((SDL_Semaphore*)sem);
  profile_func_end;
}

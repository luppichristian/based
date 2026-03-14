// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/condvar.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func condvar _condvar_create(callsite site) {
  profile_func_begin;
  condvar handle = (condvar)SDL_CreateCondition();
  if (handle == NULL) {
    thread_log_error("Failed to create condvar error=%s", SDL_GetError());
    profile_func_end;
    return NULL;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_CONDVAR,
      .object_ptr = handle,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Condvar creation was suspended handle=%p", handle);
    SDL_DestroyCondition((SDL_Condition*)handle);
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created condvar handle=%p", handle);
  profile_func_end;
  return handle;
}

func b32 _condvar_destroy(condvar cond, callsite site) {
  profile_func_begin;
  if (!cond) {
    thread_log_warn("Skipping condvar destroy for invalid handle");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_CONDVAR,
      .object_ptr = cond,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Condvar destruction was suspended handle=%p", cond);
    profile_func_end;
    return false;
  }

  thread_log_trace("Destroyed condvar handle=%p", cond);
  SDL_DestroyCondition((SDL_Condition*)cond);
  profile_func_end;
  return true;
}

func b32 condvar_is_valid(condvar cond) {
  return cond != NULL;
}

func void condvar_wait(condvar cond, mutex mtx) {
  profile_func_begin;
  if (cond == NULL || mtx == NULL) {
    thread_log_error("Rejected condvar wait cond=%p mutex=%p", cond, mtx);
    profile_func_end;
    return;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  SDL_WaitCondition((SDL_Condition*)cond, (SDL_Mutex*)mtx);
  profile_func_end;
}

func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis) {
  profile_func_begin;
  if (cond == NULL || mtx == NULL) {
    thread_log_error("Rejected condvar wait timeout cond=%p mutex=%p millis=%u", cond, mtx, millis);
    profile_func_end;
    return false;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  profile_func_end;
  return SDL_WaitConditionTimeout((SDL_Condition*)cond, (SDL_Mutex*)mtx, (Sint32)millis);
}

func void condvar_signal(condvar cond) {
  profile_func_begin;
  if (cond == NULL) {
    thread_log_error("Rejected condvar signal for invalid handle");
    profile_func_end;
    return;
  }
  assert(cond != NULL);
  SDL_SignalCondition((SDL_Condition*)cond);
  profile_func_end;
}

func void condvar_broadcast(condvar cond) {
  profile_func_begin;
  if (cond == NULL) {
    thread_log_error("Rejected condvar broadcast for invalid handle");
    profile_func_end;
    return;
  }
  assert(cond != NULL);
  SDL_BroadcastCondition((SDL_Condition*)cond);
  profile_func_end;
}

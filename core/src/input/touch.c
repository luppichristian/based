// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/touch.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 touch_is_available(void) {
  profile_func_begin;
  profile_func_end;
  return touch_get_count() > 0;
}

func sz touch_get_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func b32 touch_get_device_id(sz idx, device_id* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);
  b32 found = ids && idx < (sz)count;

  if (out_id) {
    *out_id = (device_id) {0};
  }

  if (found && out_id) {
    out_id->type = DEVICE_TYPE_TOUCH;
    out_id->instance = (u64)ids[idx];
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return found;
}

func touch_device_kind touch_get_device_kind(device_id id) {
  profile_func_begin;
  if (id.type != DEVICE_TYPE_TOUCH) {
    profile_func_end;
    return TOUCH_DEVICE_INVALID;
  }

  profile_func_end;
  return (touch_device_kind)SDL_GetTouchDeviceType((SDL_TouchID)id.instance);
}

func sz touch_get_finger_count(device_id id) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;

  if (id.type != DEVICE_TYPE_TOUCH) {
    profile_func_end;
    return 0;
  }

  fingers = SDL_GetTouchFingers((SDL_TouchID)id.instance, &count);
  if (fingers) {
    SDL_free(fingers);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func b32 touch_get_finger(device_id id, sz idx, touch_finger_state* out_finger) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;
  b32 found = 0;

  if (out_finger) {
    *out_finger = (touch_finger_state) {0};
  }

  if (id.type != DEVICE_TYPE_TOUCH || !out_finger) {
    profile_func_end;
    return 0;
  }
  assert(out_finger != NULL);

  fingers = SDL_GetTouchFingers((SDL_TouchID)id.instance, &count);
  if (fingers && idx < (sz)count && fingers[idx]) {
    out_finger->id = (finger_id)fingers[idx]->id;
    out_finger->x = fingers[idx]->x;
    out_finger->y = fingers[idx]->y;
    out_finger->pressure = fingers[idx]->pressure;
    found = 1;
  }

  if (fingers) {
    SDL_free(fingers);
  }

  profile_func_end;
  return found;
}

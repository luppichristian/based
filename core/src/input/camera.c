// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/camera.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct camera_state_entry {
  SDL_CameraID camera_id;
  b32 is_open;
  b32 is_started;
} camera_state_entry;

const_var sz CAMERA_STATE_CAP = 64;
global_var camera_state_entry camera_states[CAMERA_STATE_CAP] = {0};

func camera_state_entry* camera_find_state(SDL_CameraID camera_id, b32 create_if_missing) {
  profile_func_begin;
  for (sz item_idx = 0; item_idx < CAMERA_STATE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == camera_id) {
      profile_func_end;
      return &camera_states[item_idx];
    }
  }
  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }
  for (sz item_idx = 0; item_idx < CAMERA_STATE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == 0) {
      camera_states[item_idx].camera_id = camera_id;
      profile_func_end;
      return &camera_states[item_idx];
    }
  }
  profile_func_end;
  return NULL;
}

func b32 camera_id_is_valid(camera src) {
  return src != NULL;
}

func camera camera_from_native_id(up native_id) {
  profile_func_begin;
  profile_func_end;
  return (camera)(up)native_id;
}

func up camera_to_native_id(camera src) {
  profile_func_begin;
  profile_func_end;
  return (up)src;
}

func sz camera_get_count(void) {
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 camera_get_id(sz idx, camera* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = camera_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return found;
}

func cstr8 camera_get_name(camera id) {
  if (!camera_id_is_valid(id)) {
      return NULL;
  }

  return SDL_GetCameraName((SDL_CameraID)camera_to_native_id(id));
}

func camera_position camera_get_position(camera id) {
  if (!camera_id_is_valid(id)) {
      return CAMERA_POSITION_UNKNOWN;
  }

  switch (SDL_GetCameraPosition((SDL_CameraID)camera_to_native_id(id))) {
    case SDL_CAMERA_POSITION_FRONT_FACING:
          return CAMERA_POSITION_FRONT_FACING;
    case SDL_CAMERA_POSITION_BACK_FACING:
          return CAMERA_POSITION_BACK_FACING;
    case SDL_CAMERA_POSITION_UNKNOWN:
    default:
          return CAMERA_POSITION_UNKNOWN;
  }
}

func b32 camera_open(camera id) {
  profile_func_begin;
  if (!camera_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  state->is_open = 1;
  profile_func_end;
  return true;
}

func b32 camera_close(camera id) {
  profile_func_begin;
  if (!camera_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  *state = (camera_state_entry) {0};
  profile_func_end;
  return true;
}

func b32 camera_start(camera id) {
  profile_func_begin;
  if (!camera_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    profile_func_end;
    return false;
  }
  state->is_started = 1;
  profile_func_end;
  return true;
}

func b32 camera_stop(camera id) {
  profile_func_begin;
  if (!camera_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  state->is_started = 0;
  profile_func_end;
  return true;
}

func b32 camera_read(camera id, buffer* out_frame) {
  profile_func_begin;
  if (out_frame != NULL) {
    *out_frame = (buffer) {0};
  }
  if (!camera_id_is_valid(id) || out_frame == NULL) {
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return false;
}

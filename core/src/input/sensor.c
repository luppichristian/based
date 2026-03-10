// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/sensor.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct sensor_state_entry {
  SDL_SensorID sensor_id;
  b32 is_open;
  b32 is_started;
} sensor_state_entry;

const_var sz SENSOR_STATE_CAP = 128;
global_var sensor_state_entry sensor_states[SENSOR_STATE_CAP] = {0};

func sensor_state_entry* sensor_find_state(SDL_SensorID sensor_id, b32 create_if_missing) {
  profile_func_begin;
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == sensor_id) {
      profile_func_end;
      return &sensor_states[item_idx];
    }
  }
  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == 0) {
      sensor_states[item_idx].sensor_id = sensor_id;
      profile_func_end;
      return &sensor_states[item_idx];
    }
  }
  profile_func_end;
  return NULL;
}

func b32 sensor_id_is_valid(sensor src) {
  return src != NULL;
}

func sensor sensor_from_native_id(up native_id) {
  profile_func_begin;
  profile_func_end;
  return (sensor)(up)native_id;
}

func up sensor_to_native_id(sensor src) {
  profile_func_begin;
  profile_func_end;
  return (up)src;
}

func sz sensor_get_count(void) {
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 sensor_get_id(sz idx, sensor* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = sensor_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return found;
}

func cstr8 sensor_get_name(sensor id) {
  if (!sensor_id_is_valid(id)) {
      return NULL;
  }

  return SDL_GetSensorNameForID((SDL_SensorID)sensor_to_native_id(id));
}

func sensor_kind sensor_get_kind(sensor id) {
  if (!sensor_id_is_valid(id)) {
      return SENSOR_KIND_INVALID;
  }

  return (sensor_kind)SDL_GetSensorTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func i32 sensor_get_non_portable_kind(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    profile_func_end;
    return -1;
  }

  profile_func_end;
  return (i32)SDL_GetSensorNonPortableTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func b32 sensor_open(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  state->is_open = 1;
  profile_func_end;
  return true;
}

func b32 sensor_close(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  *state = (sensor_state_entry) {0};
  profile_func_end;
  return true;
}

func b32 sensor_start(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    profile_func_end;
    return false;
  }
  state->is_started = 1;
  profile_func_end;
  return true;
}

func b32 sensor_stop(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    profile_func_end;
    return false;
  }
  state->is_started = 0;
  profile_func_end;
  return true;
}

func b32 sensor_read(sensor id, buffer* out_samples) {
  profile_func_begin;
  if (out_samples != NULL) {
    *out_samples = (buffer) {0};
  }
  if (!sensor_id_is_valid(id) || out_samples == NULL) {
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return false;
}

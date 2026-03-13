// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/mouse.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

#define MOUSE_BUTTON_TRACK_CAP 8u
global_var u32 mouse_pressed_generation[MOUSE_BUTTON_TRACK_CAP] = {0};
global_var u32 mouse_released_generation[MOUSE_BUTTON_TRACK_CAP] = {0};
global_var u32 mouse_pressed_seen[INPUT_CAPTURE_MAX_KEYS][MOUSE_BUTTON_TRACK_CAP] = {0};
global_var u32 mouse_released_seen[INPUT_CAPTURE_MAX_KEYS][MOUSE_BUTTON_TRACK_CAP] = {0};
global_var u32 mouse_pressed_seen_epoch[INPUT_CAPTURE_MAX_KEYS][MOUSE_BUTTON_TRACK_CAP] = {0};
global_var u32 mouse_released_seen_epoch[INPUT_CAPTURE_MAX_KEYS][MOUSE_BUTTON_TRACK_CAP] = {0};

func b32 mouse_button_is_valid(u8 button) {
  return button > 0 && button < MOUSE_BUTTON_TRACK_CAP;
}

func u32 mouse_next_generation(u32 value) {
  profile_func_begin;
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  profile_func_end;
  return result;
}

func mouse_state mouse_make_state(SDL_MouseButtonFlags buttons, f32 xpos, f32 ypos) {
  profile_func_begin;
  mouse_state result = {0};
  result.x = xpos;
  result.y = ypos;
  result.button_mask = (u32)buttons;
  profile_func_end;
  return result;
}

func b32 mouse_is_available(void) {
  return SDL_HasMouse() ? true : false;
}

func b32 mouse_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_MOUSE, 0, out_id);
}

func mouse_state mouse_get_state(input_key key) {
  (void)key;
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_global_state(input_key key) {
  (void)key;
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetGlobalMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_relative_state(input_key key) {
  (void)key;
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetRelativeMouseState(&xpos, &ypos), xpos, ypos);
}

func b32 mouse_is_button_down(input_key key, u8 button) {
  profile_func_begin;
  u32 mask = mouse_get_state(key).button_mask;
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return false;
  }
  assert(mouse_button_is_valid(button));

  profile_func_end;
  return (mask & (1u << (button - 1))) != 0 ? true : false;
}

func b32 mouse_is_button_pressed(input_key key, u8 button) {
  profile_func_begin;
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return false;
  }
  assert(mouse_button_is_valid(button));

  sz slot_idx = input_capture_get_slot(key);
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(slot_idx);
  if (mouse_pressed_seen_epoch[slot_idx][button] != slot_epoch) {
    mouse_pressed_seen_epoch[slot_idx][button] = slot_epoch;
    mouse_pressed_seen[slot_idx][button] = mouse_pressed_generation[button];
    profile_func_end;
    return false;
  }

  u32 generation = mouse_pressed_generation[button];
  if (generation == 0 || mouse_pressed_seen[slot_idx][button] == generation) {
    profile_func_end;
    return false;
  }

  mouse_pressed_seen[slot_idx][button] = generation;
  profile_func_end;
  return true;
}

func b32 mouse_is_button_released(input_key key, u8 button) {
  profile_func_begin;
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return false;
  }
  assert(mouse_button_is_valid(button));

  sz slot_idx = input_capture_get_slot(key);
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(slot_idx);
  if (mouse_released_seen_epoch[slot_idx][button] != slot_epoch) {
    mouse_released_seen_epoch[slot_idx][button] = slot_epoch;
    mouse_released_seen[slot_idx][button] = mouse_released_generation[button];
    profile_func_end;
    return false;
  }

  u32 generation = mouse_released_generation[button];
  if (generation == 0 || mouse_released_seen[slot_idx][button] == generation) {
    profile_func_end;
    return false;
  }

  mouse_released_seen[slot_idx][button] = generation;
  profile_func_end;
  return true;
}

func void mouse_internal_on_msg(msg* src) {
  profile_func_begin;
  if (src == NULL || (src->type != MSG_CORE_TYPE_MOUSE_BUTTON_DOWN && src->type != MSG_CORE_TYPE_MOUSE_BUTTON_UP)) {
    profile_func_end;
    return;
  }

  u8 button = (u8)msg_core_get_mouse_button(src)->button;
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return;
  }

  if (src->type == MSG_CORE_TYPE_MOUSE_BUTTON_DOWN) {
    mouse_pressed_generation[button] = mouse_next_generation(mouse_pressed_generation[button]);
  } else {
    mouse_released_generation[button] = mouse_next_generation(mouse_released_generation[button]);
  }
  profile_func_end;
}

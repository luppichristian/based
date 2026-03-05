// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/mouse.h"
#include "input/msg.h"
#include "../sdl3_include.h"

#define MOUSE_BUTTON_TRACK_CAP 8u
global_var b32 mouse_pressed_latched[MOUSE_BUTTON_TRACK_CAP] = {0};
global_var b32 mouse_released_latched[MOUSE_BUTTON_TRACK_CAP] = {0};

func b32 mouse_button_is_valid(u8 button) {
  return button > 0 && button < MOUSE_BUTTON_TRACK_CAP;
}

func mouse_state mouse_make_state(SDL_MouseButtonFlags buttons, f32 xpos, f32 ypos) {
  mouse_state result = {0};
  result.x = xpos;
  result.y = ypos;
  result.button_mask = (u32)buttons;
  return result;
}

func b32 mouse_is_available(void) {
  return SDL_HasMouse() ? 1 : 0;
}

func b32 mouse_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_MOUSE, 0, out_id);
}

func mouse_state mouse_get_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_global_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetGlobalMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_relative_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetRelativeMouseState(&xpos, &ypos), xpos, ypos);
}

func b32 mouse_is_button_down(u8 button) {
  u32 mask = mouse_get_state().button_mask;
  if (!mouse_button_is_valid(button)) {
    return 0;
  }

  return (mask & (1u << (button - 1))) != 0 ? 1 : 0;
}

func b32 mouse_is_button_pressed(u8 button) {
  if (!mouse_button_is_valid(button)) {
    return 0;
  }

  b32 pressed = mouse_pressed_latched[button];
  mouse_pressed_latched[button] = 0;
  return pressed;
}

func b32 mouse_is_button_released(u8 button) {
  if (!mouse_button_is_valid(button)) {
    return 0;
  }

  b32 released = mouse_released_latched[button];
  mouse_released_latched[button] = 0;
  return released;
}

func void mouse_internal_on_msg(const msg* src) {
  if (src == NULL || (src->type != MSG_TYPE_MOUSE_BUTTON_DOWN && src->type != MSG_TYPE_MOUSE_BUTTON_UP)) {
    return;
  }

  u8 button = (u8)src->mouse_button.button;
  if (!mouse_button_is_valid(button)) {
    return;
  }

  if (src->type == MSG_TYPE_MOUSE_BUTTON_DOWN) {
    mouse_pressed_latched[button] = 1;
    mouse_released_latched[button] = 0;
  } else {
    mouse_released_latched[button] = 1;
    mouse_pressed_latched[button] = 0;
  }
}

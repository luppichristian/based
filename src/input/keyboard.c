// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "input/msg.h"
#include "../sdl3_include.h"

global_var b32 keyboard_pressed_latched[SDL_SCANCODE_COUNT] = {0};
global_var b32 keyboard_released_latched[SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_repeat_count[SDL_SCANCODE_COUNT] = {0};

func b32 keyboard_scancode_is_valid(u32 scancode) {
  return scancode < (u32)SDL_SCANCODE_COUNT;
}

func b32 keyboard_is_available(void) {
  return SDL_HasKeyboard() ? 1 : 0;
}

func b32 keyboard_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_KEYBOARD, 0, out_id);
}

func b32 keyboard_is_key_down(u32 scancode) {
  int key_count = 0;
  const bool* state = SDL_GetKeyboardState(&key_count);

  if (!state || scancode >= (u32)key_count) {
    return 0;
  }

  return state[scancode] ? 1 : 0;
}

func b32 keyboard_is_key_pressed(u32 scancode) {
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }

  b32 pressed = keyboard_pressed_latched[scancode];
  keyboard_pressed_latched[scancode] = 0;
  return pressed;
}

func b32 keyboard_is_key_released(u32 scancode) {
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }

  b32 released = keyboard_released_latched[scancode];
  keyboard_released_latched[scancode] = 0;
  return released;
}

func u32 keyboard_get_key_repeat_count(u32 scancode) {
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }

  return keyboard_repeat_count[scancode];
}

func u16 keyboard_get_mods(void) {
  return (u16)SDL_GetModState();
}

func b32 keyboard_has_mods(u16 required_mods) {
  u16 current_mods = keyboard_get_mods();
  return (current_mods & required_mods) == required_mods;
}

func b32 keyboard_has_mods_exact(u16 required_mods, u16 forbidden_mods) {
  u16 current_mods = keyboard_get_mods();
  return ((current_mods & required_mods) == required_mods) && ((current_mods & forbidden_mods) == 0);
}

func b32 keyboard_is_key_down_mod(u32 scancode, u16 required_mods, u16 forbidden_mods) {
  return keyboard_is_key_down(scancode) && keyboard_has_mods_exact(required_mods, forbidden_mods);
}

func u32 keyboard_get_keycode(u32 scancode, u16 modifiers, b32 key_event) {
  return (u32)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
}

func cstr8 keyboard_get_scancode_name(u32 scancode) {
  return SDL_GetScancodeName((SDL_Scancode)scancode);
}

func void keyboard_internal_on_msg(const msg* src) {
  if (src == NULL || (src->type != MSG_TYPE_KEY_DOWN && src->type != MSG_TYPE_KEY_UP)) {
    return;
  }

  if (!keyboard_scancode_is_valid(src->keyboard.scancode)) {
    return;
  }

  u32 scancode = src->keyboard.scancode;

  if (src->type == MSG_TYPE_KEY_DOWN) {
    keyboard_pressed_latched[scancode] = src->keyboard.repeat ? 0 : 1;
    keyboard_released_latched[scancode] = 0;
    if (src->keyboard.repeat) {
      keyboard_repeat_count[scancode] += 1;
    } else {
      keyboard_repeat_count[scancode] = 0;
    }
  } else if (src->type == MSG_TYPE_KEY_UP) {
    keyboard_released_latched[scancode] = 1;
    keyboard_pressed_latched[scancode] = 0;
    keyboard_repeat_count[scancode] = 0;
  }
}

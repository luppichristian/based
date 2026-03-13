// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "interface/window.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 keyboard_internal_scancode_is_valid(u32 scancode) {
  return scancode < (u32)SDL_SCANCODE_COUNT;
}

func b32 keyboard_vkey_is_valid(vkey key) {
  switch (key) {
#define BASED_VKEY_CASE(name, value) case name:
    BASED_VKEY_LIST(BASED_VKEY_CASE)
#undef BASED_VKEY_CASE
    return true;
    default:
      return false;
  }
}

func u32 keyboard_internal_scancode_from_vkey(vkey key) {
  profile_func_begin;
  if (!keyboard_vkey_is_valid(key)) {
    profile_func_end;
    return 0;
  }

  u32 scancode = (u32)key;
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return scancode;
}

func vkey keyboard_internal_vkey_from_scancode(u32 scancode) {
  profile_func_begin;
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return VKEY_UNKNOWN;
  }

  vkey key = (vkey)scancode;
  if (!keyboard_vkey_is_valid(key)) {
    profile_func_end;
    return VKEY_UNKNOWN;
  }

  profile_func_end;
  return key;
}

func b32 keyboard_is_available(void) {
  return SDL_HasKeyboard() ? true : false;
}

func device keyboard_get_primary_device(void) {
  return devices_get_device(DEVICE_TYPE_KEYBOARD, 0);
}

func b32 keyboard_is_key_down(vkey key) {
  profile_func_begin;
  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  int key_count = 0;
  const bool* state = SDL_GetKeyboardState(&key_count);

  if (!state || scancode >= (u32)key_count) {
    profile_func_end;
    return false;
  }
  assert(scancode < (u32)key_count);

  profile_func_end;
  return state[scancode] ? true : false;
}

func keymod keyboard_get_mods(void) {
  return (keymod)SDL_GetModState();
}

func b32 keyboard_has_mods(keymod required_mods) {
  keymod current_mods = keyboard_get_mods();
  return (current_mods & required_mods) == required_mods;
}

func b32 keyboard_has_mods_exact(keymod required_mods, keymod forbidden_mods) {
  keymod current_mods = keyboard_get_mods();
  return ((current_mods & required_mods) == required_mods) && ((current_mods & forbidden_mods) == 0);
}

func b32 keyboard_is_key_down_mod(vkey key, keymod required_mods, keymod forbidden_mods) {
  profile_func_begin;
  b32 result = keyboard_is_key_down(key) && keyboard_has_mods_exact(required_mods, forbidden_mods);
  profile_func_end;
  return result;
}

func i32 keyboard_internal_keycode_from_vkey(vkey key, keymod modifiers, b32 key_event) {
  profile_func_begin;
  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return 0;
  }

  i32 result = (i32)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
  profile_func_end;
  return result;
}

func cstr8 keyboard_get_key_name(vkey key) {
  profile_func_begin;
  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return "";
  }

  cstr8 result = SDL_GetScancodeName((SDL_Scancode)scancode);
  profile_func_end;
  return result;
}

func cstr8 keyboard_get_key_display_name(vkey key, keymod modifiers, b32 key_event) {
  profile_func_begin;
  i32 keycode = keyboard_internal_keycode_from_vkey(key, modifiers, key_event);
  if (keycode == 0) {
    profile_func_end;
    return "";
  }

  cstr8 result = SDL_GetKeyName((SDL_Keycode)keycode);
  profile_func_end;
  return result;
}

func b32 keyboard_start_text_input(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StartTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 keyboard_stop_text_input(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StopTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 keyboard_is_text_input_active(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_TextInputActive(window_ptr) ? true : false;
  profile_func_end;
  return result;
}

func b32 keyboard_set_text_input_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  SDL_Rect area = {.x = xpos, .y = ypos, .w = width, .h = height};
  b32 result = SDL_SetTextInputArea(window_ptr, &area, 0);
  profile_func_end;
  return result;
}

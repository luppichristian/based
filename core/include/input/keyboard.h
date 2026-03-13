// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../interface/window.h"
#include "devices.h"
#include "vkeys.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Keyboard
// =========================================================================

// Returns 1 if key is a known cross-platform virtual key, 0 otherwise.
func b32 keyboard_vkey_is_valid(vkey key);

// Returns 1 if a keyboard backend is available, 0 otherwise.
func b32 keyboard_is_available(void);

// Returns the primary keyboard device, or NULL when unavailable.
func device keyboard_get_primary_device(void);

// Returns 1 if the key identified by key is currently pressed, 0 otherwise.
func b32 keyboard_is_key_down(vkey key);

// Returns the current keyboard modifier bitmask.
func keymod keyboard_get_mods(void);

// Returns 1 when all required_mods are active in the current modifier state.
func b32 keyboard_has_mods(keymod required_mods);

// Returns 1 when required_mods are active and forbidden_mods are inactive.
func b32 keyboard_has_mods_exact(keymod required_mods, keymod forbidden_mods);

// Returns 1 when key is down and modifier requirements are satisfied.
func b32 keyboard_is_key_down_mod(vkey key, keymod required_mods, keymod forbidden_mods);

// Returns a readable physical-key name for key.
func cstr8 keyboard_get_key_name(vkey key);

// Returns a readable translated key name for key under the supplied keyboard layout state.
func cstr8 keyboard_get_key_display_name(vkey key, keymod modifiers, b32 key_event);

// IME/text-input control for an optional target window.
func b32 keyboard_start_text_input(window opt_window);
func b32 keyboard_stop_text_input(window opt_window);
func b32 keyboard_is_text_input_active(window opt_window);
func b32 keyboard_set_text_input_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height);

// =========================================================================
c_end;
// =========================================================================

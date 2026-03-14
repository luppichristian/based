// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "icon.h"
#include "window.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct cursor_pos {
  f32 x;
  f32 y;
} cursor_pos;

// Sets the active cursor graphic from icon_id. System icons and RGBA icons are both supported.
// Returns 1 on success, 0 otherwise.
func b32 cursor_set_icon(icon icon_id);

// Restores the backend default cursor icon. Returns 1 on success, 0 otherwise.
func b32 cursor_reset_icon(void);

// Shows or hides the active cursor. Returns 1 on success, 0 otherwise.
func b32 cursor_set_visible(b32 visible);

// Returns 1 if the cursor is currently visible, 0 otherwise.
func b32 cursor_is_visible(void);

// Enables or disables cursor capture for opt_window. When opt_window is NULL, the current mouse-focus window is used.
// Returns 1 on success, 0 otherwise.
func b32 cursor_set_capture(window opt_window, b32 enabled);

// Returns 1 if opt_window currently has cursor capture, 0 otherwise.
func b32 cursor_is_captured(window opt_window);

// Enables or disables relative mouse mode for opt_window. When opt_window is NULL, the current mouse-focus window is used.
// Returns 1 on success, 0 otherwise.
func b32 cursor_set_relative_mode(window opt_window, b32 enabled);

// Returns 1 if opt_window is currently in relative mouse mode, 0 otherwise.
func b32 cursor_is_relative_mode(window opt_window);

// Returns the current cursor pos in the active window coordinate space.
func cursor_pos cursor_get_pos(void);

// Returns the current cursor pos in desktop-global coordinates.
func cursor_pos cursor_get_global_pos(void);

// Returns the relative cursor delta accumulated since the last pump.
func cursor_pos cursor_get_relative_pos(void);

// Warps the cursor to xpos/ypos. Coordinates are window-relative when opt_window is valid, otherwise desktop-global.
// Returns 1 on success, 0 otherwise.
func b32 cursor_warp(window opt_window, f32 xpos, f32 ypos);

// =========================================================================
c_end;
// =========================================================================

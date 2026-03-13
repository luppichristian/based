// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "icon.h"
#include "monitor.h"

// =========================================================================
c_begin;
// =========================================================================

typedef void* window;

typedef enum window_mode {
  WINDOW_MODE_WINDOWED = 0,
  WINDOW_MODE_UNDECORATED = 1,
  WINDOW_MODE_FULLSCREEN = 2,
  WINDOW_MODE_WINDOWED_FULLSCREEN = 3,
} window_mode;

typedef enum window_center_axis {
  WINDOW_CENTER_AXIS_NONE = 0,
  WINDOW_CENTER_AXIS_HORIZONTAL = 1,
  WINDOW_CENTER_AXIS_VERTICAL = 2,
  WINDOW_CENTER_AXIS_BOTH = 3,
} window_center_axis;

// Identifier conversion helpers.
// Returns 1 if src is a non-null window identifier, 0 otherwise.
func b32 window_id_is_valid(window src);

// Global window enumeration.
// Returns the number of currently known windows.
func sz window_get_total_count(void);
func window window_get_from_idx(sz idx);

// Returns 1 if window_id currently resolves to a live window, 0 otherwise.
func b32 window_is_valid(window window_id);

// Creation and destruction.
// Creates a window with title, size, mode, and backend flags. Returns a valid window on success, NULL otherwise.
func window window_create(cstr8 title, i32 width, i32 height, window_mode mode, u64 flags);

// Destroys window_id. Returns 1 on success, 0 otherwise.
func b32 window_destroy(window window_id);

// Visibility.
// Returns 1 when window_id is hidden, 0 otherwise.
func b32 window_is_hidden(window window_id);

// Shows window_id. Returns 1 on success, 0 otherwise.
func b32 window_show(window window_id);

// Hides window_id. Returns 1 on success, 0 otherwise.
func b32 window_hide(window window_id);

// Position.
// Moves window_id to xpos/ypos. Returns 1 on success, 0 otherwise.
func b32 window_set_pos(window window_id, i32 xpos, i32 ypos);

// Writes the current window position into out_xpos/out_ypos. Returns 1 on success, 0 otherwise.
func b32 window_get_pos(window window_id, i32* out_xpos, i32* out_ypos);

// Size.
// Resizes window_id to width/height. Returns 1 on success, 0 otherwise.
func b32 window_set_size(window window_id, i32 width, i32 height);

// Writes the current window size into out_width/out_height. Returns 1 on success, 0 otherwise.
func b32 window_get_size(window window_id, i32* out_width, i32* out_height);

// Window mode.
// Changes window_id to mode. Returns 1 on success, 0 otherwise.
func b32 window_set_mode(window window_id, window_mode mode);

// Returns the current canonical mode for window_id.
func window_mode window_get_mode(window window_id);

// Window-state controls.
// Returns 1 when window_id is minimized, 0 otherwise.
func b32 window_is_minimized(window window_id);

// Returns 1 when window_id is maximized, 0 otherwise.
func b32 window_is_maximized(window window_id);

// Returns 1 when window_id has input focus, 0 otherwise.
func b32 window_has_input_focus(window window_id);

// Returns 1 when window_id has mouse focus, 0 otherwise.
func b32 window_has_mouse_focus(window window_id);

// Returns 1 when window_id is marked always-on-top, 0 otherwise.
func b32 window_is_topmost(window window_id);

// Minimizes window_id. Returns 1 on success, 0 otherwise.
func b32 window_minimize(window window_id);

// Maximizes window_id. Returns 1 on success, 0 otherwise.
func b32 window_maximize(window window_id);

// Restores window_id from a minimized or maximized state. Returns 1 on success, 0 otherwise.
func b32 window_restore(window window_id);

// Raises and focuses window_id. Returns 1 on success, 0 otherwise.
func b32 window_focus(window window_id);

// Enables or disables the always-on-top state for window_id. Returns 1 on success, 0 otherwise.
func b32 window_set_topmost(window window_id, b32 enabled);

// Metadata.
// Returns the current title string for window_id, or NULL on failure.
func cstr8 window_get_title(window window_id);

// Sets the title of window_id to title. Returns 1 on success, 0 otherwise.
func b32 window_set_title(window window_id, cstr8 title);

// Sets the window icon from icon_id. RGBA-backed icons are supported. Returns 1 on success, 0 otherwise.
func b32 window_set_icon(window window_id, icon icon_id);

// Writes the current monitor containing window_id into out_monitor_id. Returns 1 on success, 0 otherwise.
func b32 window_get_monitor(window window_id, monitor* out_monitor_id);

// Writes the monitor where window_id was first created into out_monitor_id. Returns 1 on success, 0 otherwise.
func b32 window_get_creation_monitor(window window_id, monitor* out_monitor_id);

// Centers window_id within monitor_id along the axes selected by axis_mask. When monitor_id is invalid, the current or primary monitor is used.
// Returns 1 on success, 0 otherwise.
func b32 window_center_in_monitor(
    window window_id,
    monitor monitor_id,
    window_center_axis axis_mask);

// =========================================================================
c_end;
// =========================================================================

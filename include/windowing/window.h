// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "display.h"

typedef void* window;

// Returns 1 if src refers to a concrete window id, 0 otherwise.
func b32 window_id_is_valid(window src);

// Builds a window from a backend-native window identifier.
func window window_from_native_id(up native_id);

// Returns the backend-native window identifier encoded in src.
func up window_to_native_id(window src);

// Returns the number of currently known windows.
func sz window_get_count(void);

// Writes the window id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 window_get_id(sz idx, window* out_id);

// Returns 1 when id maps to an existing window, 0 otherwise.
func b32 window_is_valid(window id);

// Returns a backend-defined title for id, or NULL when unavailable.
func cstr8 window_get_title(window id);

// Writes the display currently associated with id into out_display_id. Returns 1 on success, 0 otherwise.
func b32 window_get_display_id(window id, display* out_display_id);

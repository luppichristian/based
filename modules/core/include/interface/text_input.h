// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "window.h"

// =========================================================================
c_begin;
// =========================================================================

// IME/text-input control for an optional target window.
func b32 text_input_start(window opt_window);
func b32 text_input_stop(window opt_window);
func b32 text_input_is_active(window opt_window);
func b32 text_input_set_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height);

// =========================================================================
c_end;
// =========================================================================
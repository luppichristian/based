// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../strings/cstrings.h"
#include "cpu_info.h"
#include "system_runtime.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct hardware_info {
  // Static CPU identity and instruction capability snapshot.
  cpu_info cpu;

  // Total physical memory visible to the operating system.
  sz memory_total;

  // Currently available physical memory reported by the host.
  sz memory_available;

  // Number of currently visible monitors.
  u32 monitor_count;

  // Number of connected keyboards visible to the backend.
  u32 keyboard_count;

  // Number of connected mice visible to the backend.
  u32 mouse_count;

  // Number of connected gamepads visible to the backend.
  u32 gamepad_count;

  // Active SDL video driver name, or a stable fallback when unavailable.
  str8_short video_driver_name;

  // Backend-reported name for the first available keyboard.
  str8_short primary_keyboard_name;

  // Backend-reported name for the first available mouse.
  str8_short primary_mouse_name;
} hardware_info;

// Queries a coarse hardware inventory snapshot for the current machine.
func b32 hardware_info_query(hardware_info* out_info);

// =========================================================================
c_end;
// =========================================================================

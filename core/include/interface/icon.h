// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

typedef void* icon;

typedef enum icon_system {
  ICON_SYSTEM_DEFAULT = 0,
  ICON_SYSTEM_TEXT = 1,
  ICON_SYSTEM_WAIT = 2,
  ICON_SYSTEM_CROSSHAIR = 3,
  ICON_SYSTEM_PROGRESS = 4,
  ICON_SYSTEM_NWSE_RESIZE = 5,
  ICON_SYSTEM_NESW_RESIZE = 6,
  ICON_SYSTEM_EW_RESIZE = 7,
  ICON_SYSTEM_NS_RESIZE = 8,
  ICON_SYSTEM_MOVE = 9,
  ICON_SYSTEM_NOT_ALLOWED = 10,
  ICON_SYSTEM_POINTER = 11,
  ICON_SYSTEM_NW_RESIZE = 12,
  ICON_SYSTEM_N_RESIZE = 13,
  ICON_SYSTEM_NE_RESIZE = 14,
  ICON_SYSTEM_E_RESIZE = 15,
  ICON_SYSTEM_SE_RESIZE = 16,
  ICON_SYSTEM_S_RESIZE = 17,
  ICON_SYSTEM_SW_RESIZE = 18,
  ICON_SYSTEM_W_RESIZE = 19,
} icon_system;

func b32 icon_id_is_valid(icon src);

// Creates an icon from tightly packed RGBA8 pixel data. Returns a valid icon on success, NULL otherwise.
func icon icon_create_rgba(i32 width, i32 height, const u8* rgba_pixels, i32 hot_xpos, i32 hot_ypos);

// Creates an icon from a backend/system default kind. Returns a valid icon on success, NULL otherwise.
func icon icon_create_system(icon_system system_icon);

// Destroys icon_id. Returns 1 on success, 0 otherwise.
func b32 icon_destroy(icon icon_id);

// Writes the icon dimensions into out_width/out_height. Returns 1 on success, 0 otherwise.
func b32 icon_get_size(icon icon_id, i32* out_width, i32* out_height);

// Writes the icon hotspot into out_xpos/out_ypos. Returns 1 on success, 0 otherwise.
func b32 icon_get_hotspot(icon icon_id, i32* out_xpos, i32* out_ypos);

// Writes the system icon kind into out_system_icon when icon_id is system-backed. Returns 1 on success, 0 otherwise.
func b32 icon_get_system(icon icon_id, icon_system* out_system_icon);

// Returns 1 when icon_id is backed by RGBA pixel data, 0 otherwise.
func b32 icon_is_rgba(icon icon_id);

// Returns 1 when icon_id is backed by a system icon kind, 0 otherwise.
func b32 icon_is_system(icon icon_id);

// =========================================================================
c_end;
// =========================================================================

// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct language_info {
  // Preferred desktop/UI language as a normalized BCP-47-like tag such as en-US.
  str8_short desktop_language;

  // Active keyboard language as a normalized BCP-47-like tag when the platform exposes it.
  str8_short keyboard_language;

  // Keyboard layout identifier or descriptive backend label when available.
  str8_short keyboard_layout_name;
} language_info;

// Queries the current desktop language and active keyboard language/layout.
func b32 language_query(language_info* out_info);

// =========================================================================
c_end;
// =========================================================================

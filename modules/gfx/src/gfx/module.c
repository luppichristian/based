// MIT License
// Copyright (c) 2026 Christian Luppi

#include "gfx/module.h"

func b32 gfx_module_is_available(void) {
  return true;
}

func cstr8 gfx_module_get_name(void) {
  return "gfx";
}

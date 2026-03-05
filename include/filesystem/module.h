// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

// Generic function pointer returned by module_get_func().
typedef void module_func(void);

// Signature expected from module entry points defined by basic/entry.h.
typedef b32 module_init_func(void);
typedef void module_quit_func(void);

typedef struct mod {
  void* native_handle;
  module_quit_func* quit_func;
  path source_path;
  b32 initialized;
} mod;

// Loads src as a dynamic module and calls module_init(). Returns an empty
// module on failure.
func mod module_open(const path* src);

// Returns 1 when mod currently references a loaded module.
func b32 module_is_open(const mod* mod_ptr);

// Returns the named symbol as a generic function pointer, or NULL on failure.
func module_func* module_get_func(const mod* mod_ptr, cstr8 name);

// Calls module_quit() when needed, unloads the module, and resets mod.
func void module_close(mod* mod_ptr);

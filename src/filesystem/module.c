// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/module.h"

#include "basic/env_defines.h"

#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#  include <dlfcn.h>
#endif

#define MODULE_INIT_SYMBOL "mod_init"
#define MODULE_QUIT_SYMBOL "mod_quit"

func mod module_empty(void) {
  mod module_value;
  memset(&module_value, 0, size_of(module_value));
  module_value.source_path = path_from_cstr("");
  return module_value;
}

func b32 module_is_open(const mod* mod_ptr) {
  if (mod_ptr == NULL) {
    return 0;
  }

  return mod_ptr->native_handle != NULL ? 1 : 0;
}

func module_func* module_get_func(const mod* mod_ptr, cstr8 name) {
  if (!module_is_open(mod_ptr) || name == NULL || name[0] == '\0') {
    return NULL;
  }

#if defined(PLATFORM_WINDOWS)
  FARPROC raw_symbol = GetProcAddress((HMODULE)mod_ptr->native_handle, name);
  union {
    FARPROC raw_symbol;
    module_func* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  return cast_value.resolved_func;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  void* raw_symbol = dlsym(mod_ptr->native_handle, name);
  union {
    void* raw_symbol;
    module_func* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  return cast_value.resolved_func;
#else
  (void)name;
  return NULL;
#endif
}

func void module_close(mod* mod_ptr) {
  if (!module_is_open(mod_ptr)) {
    return;
  }

  if (mod_ptr->initialized && mod_ptr->quit_func != NULL) {
    mod_ptr->quit_func();
    mod_ptr->initialized = 0;
  }

#if defined(PLATFORM_WINDOWS)
  (void)FreeLibrary((HMODULE)mod_ptr->native_handle);
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  (void)dlclose(mod_ptr->native_handle);
#endif

  *mod_ptr = module_empty();
}

func mod module_open(const path* src) {
  mod module_value = module_empty();
  module_init_func* init_func = NULL;
  module_func* init_symbol = NULL;
  module_func* quit_symbol = NULL;

  if (src == NULL || src->buf[0] == '\0') {
    return module_value;
  }

#if defined(PLATFORM_WINDOWS)
  HMODULE handle = LoadLibraryA(src->buf);
  if (handle == NULL) {
    return module_value;
  }
  module_value.native_handle = (void*)handle;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  void* handle = dlopen(src->buf, RTLD_NOW);
  if (handle == NULL) {
    return module_value;
  }
  module_value.native_handle = handle;
#else
  return module_value;
#endif

  module_value.source_path = *src;

  init_symbol = module_get_func(&module_value, MODULE_INIT_SYMBOL);
  quit_symbol = module_get_func(&module_value, MODULE_QUIT_SYMBOL);
  union {
    module_func* generic_func;
    module_init_func* init_func;
    module_quit_func* quit_func;
  } cast_value;
  cast_value.generic_func = init_symbol;
  init_func = cast_value.init_func;
  cast_value.generic_func = quit_symbol;
  module_value.quit_func = cast_value.quit_func;

  if (init_func == NULL || module_value.quit_func == NULL) {
    module_close(&module_value);
    return module_empty();
  }

  if (!init_func()) {
    module_close(&module_value);
    return module_empty();
  }

  module_value.initialized = 1;
  return module_value;
}

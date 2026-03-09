// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/module.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"

#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(PLATFORM_UNIX)
#  include <dlfcn.h>
#endif

#define MODULE_INIT_SYMBOL "mod_init"
#define MODULE_QUIT_SYMBOL "mod_quit"

func mod module_empty(void) {
  profile_func_begin;
  mod module_value;
  memset(&module_value, 0, size_of(module_value));
  module_value.source_path = path_from_cstr("");
  profile_func_end;
  return module_value;
}

func b32 mod_is_open(const mod* mod_ptr) {
  profile_func_begin;
  if (mod_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return mod_ptr->native_handle != NULL ? 1 : 0;
}

func void* mod_get_func(const mod* mod_ptr, cstr8 name) {
  profile_func_begin;
  if (!mod_is_open(mod_ptr) || name == NULL || name[0] == '\0') {
    profile_func_end;
    return NULL;
  }
  assert(name[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  FARPROC raw_symbol = GetProcAddress((HMODULE)mod_ptr->native_handle, name);
  union {
    FARPROC raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  profile_func_end;
  return cast_value.resolved_func;
#elif defined(PLATFORM_UNIX)
  void* raw_symbol = dlsym(mod_ptr->native_handle, name);
  union {
    void* raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  profile_func_end;
  return cast_value.resolved_func;
#else
  (void)name;
  profile_func_end;
  return NULL;
#endif
  profile_func_end;
}

func cstr8 mod_get_extension(void) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS)
  profile_func_end;
  return ".dll";
#elif defined(PLATFORM_MACOS)
  profile_func_end;
  return ".dylib";
#elif defined(PLATFORM_UNIX)
  profile_func_end;
  return ".so";
#else
  profile_func_end;
  return "";
#endif
}

func void mod_close(mod* mod_ptr) {
  profile_func_begin;
  if (!mod_is_open(mod_ptr)) {
    profile_func_end;
    return;
  }
  assert(mod_ptr != NULL);

  if (mod_ptr->initialized && mod_ptr->quit_func != NULL) {
    mod_ptr->quit_func();
    mod_ptr->initialized = 0;
  }

#if defined(PLATFORM_WINDOWS)
  (void)FreeLibrary((HMODULE)mod_ptr->native_handle);
#elif defined(PLATFORM_UNIX)
  (void)dlclose(mod_ptr->native_handle);
#endif

  *mod_ptr = module_empty();
  thread_log_trace("mod_close");
  profile_func_end;
}

func mod mod_open(const path* src) {
  profile_func_begin;
  mod module_value = module_empty();
  void* init_symbol = NULL;
  void* quit_symbol = NULL;

  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return module_value;
  }
  assert(src->buf[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  HMODULE handle = LoadLibraryA(src->buf);
  if (handle == NULL) {
    profile_func_end;
    return module_value;
  }
  module_value.native_handle = (void*)handle;
#elif defined(PLATFORM_UNIX)
  void* handle = dlopen(src->buf, RTLD_NOW);
  if (handle == NULL) {
    profile_func_end;
    return module_value;
  }
  module_value.native_handle = handle;
#else
  profile_func_end;
  return module_value;
#endif

  module_value.source_path = *src;

  init_symbol = mod_get_func(&module_value, MODULE_INIT_SYMBOL);
  quit_symbol = mod_get_func(&module_value, MODULE_QUIT_SYMBOL);
  union {
    void* generic_func;
    mod_init_func* init_func;
    mod_quit_func* quit_func;
  } cast_value;
  cast_value.generic_func = init_symbol;
  module_value.init_func = cast_value.init_func;
  cast_value.generic_func = quit_symbol;
  module_value.quit_func = cast_value.quit_func;

  if (module_value.init_func == NULL || module_value.quit_func == NULL) {
    thread_log_error("mod_open: missing mod_init/mod_quit symbols path=%s", src->buf);
    mod_close(&module_value);
    profile_func_end;
    return module_empty();
  }

  if (!module_value.init_func()) {
    thread_log_error("mod_open: mod_init failed path=%s", src->buf);
    mod_close(&module_value);
    profile_func_end;
    return module_empty();
  }

  module_value.initialized = 1;
  thread_log_trace("mod_open: path=%s", src->buf);
  profile_func_end;
  return module_value;
}

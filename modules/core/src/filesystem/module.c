// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/module.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"

#include <string.h>

#define MODULE_INIT_SYMBOL "mod_init"
#define MODULE_QUIT_SYMBOL "mod_quit"

func mod module_empty(void) {
  profile_func_begin;
  mod module_value;
  mem_zero(&module_value, size_of(module_value));
  module_value.source_path = path_from_cstr("");
  profile_func_end;
  return module_value;
}

func b32 mod_is_open(const mod* mod_ptr) {
  if (mod_ptr == NULL) {
    return false;
  }

  return mod_ptr->native_handle != NULL ? true : false;
}

func void* mod_get_func(const mod* mod_ptr, cstr8 name) {
  profile_func_begin;
  if (!mod_is_open(mod_ptr) || name == NULL || name[0] == '\0') {
    thread_log_error("Rejected module symbol lookup module=%p symbol=%s",
                     (void*)mod_ptr,
                     name != NULL ? name : "<null>");
    profile_func_end;
    return NULL;
  }
  assert(name[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  FARPROC raw_symbol = GetProcAddress((HMODULE)mod_ptr->native_handle, name);
  if (raw_symbol == NULL) {
    thread_log_warn("Module symbol lookup failed path=%s symbol=%s", mod_ptr->source_path.buf, name);
  }
  union {
    FARPROC raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  profile_func_end;
  return cast_value.resolved_func;
#elif defined(PLATFORM_UNIX)
  void* raw_symbol = dlsym(mod_ptr->native_handle, name);
  if (raw_symbol == NULL) {
    thread_log_warn("Module symbol lookup failed path=%s symbol=%s error=%s",
                    mod_ptr->source_path.buf,
                    name,
                    dlerror());
  }
  union {
    void* raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  profile_func_end;
  return cast_value.resolved_func;
#else
  (void)name;
  invalid_code_path;
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
  invalid_code_path;
  profile_func_end;
  return "";
#endif
}

func void mod_close(mod* mod_ptr) {
  profile_func_begin;
  path source_path = path_from_cstr("");
  if (!mod_is_open(mod_ptr)) {
    thread_log_warn("Skipping module close for unopened module");
    profile_func_end;
    return;
  }
  assert(mod_ptr != NULL);
  source_path = mod_ptr->source_path;

  if (mod_ptr->initialized && mod_ptr->quit_func != NULL) {
    thread_log_verbose("Calling module quit path=%s", mod_ptr->source_path.buf);
    mod_ptr->quit_func();
    mod_ptr->initialized = false;
  }

#if defined(PLATFORM_WINDOWS)
  (void)FreeLibrary((HMODULE)mod_ptr->native_handle);
#elif defined(PLATFORM_UNIX)
  (void)dlclose(mod_ptr->native_handle);
#else
  invalid_code_path;
#endif

  *mod_ptr = module_empty();
  thread_log_trace("Closed module path=%s", source_path.buf);
  profile_func_end;
}

func mod mod_open(const path* src) {
  profile_func_begin;
  mod module_value = module_empty();
  void* init_symbol = NULL;
  void* quit_symbol = NULL;

  if (src == NULL || src->buf[0] == '\0') {
    thread_log_error("Rejected module open for invalid path");
    profile_func_end;
    return module_value;
  }
  assert(src->buf[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  HMODULE handle = LoadLibraryA(src->buf);
  if (handle == NULL) {
    thread_log_error("Failed to load module path=%s error=%lu", src->buf, (unsigned long)GetLastError());
    profile_func_end;
    return module_value;
  }
  module_value.native_handle = (void*)handle;
#elif defined(PLATFORM_UNIX)
  void* handle = dlopen(src->buf, RTLD_NOW);
  if (handle == NULL) {
    thread_log_error("Failed to load module path=%s error=%s", src->buf, dlerror());
    profile_func_end;
    return module_value;
  }
  module_value.native_handle = handle;
#else
  invalid_code_path;
  profile_func_end;
  return module_value;
#endif

  module_value.source_path = *src;

  init_symbol = mod_get_func(&module_value, MODULE_INIT_SYMBOL);
  quit_symbol = mod_get_func(&module_value, MODULE_QUIT_SYMBOL);
  thread_log_verbose("Resolved module lifecycle symbols path=%s init=%p quit=%p", src->buf, init_symbol, quit_symbol);
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
    thread_log_error("Missing module init or quit symbols path=%s", src->buf);
    mod_close(&module_value);
    profile_func_end;
    return module_empty();
  }

  if (!module_value.init_func()) {
    thread_log_error("Module init failed path=%s", src->buf);
    mod_close(&module_value);
    profile_func_end;
    return module_empty();
  }

  module_value.initialized = true;
  thread_log_info("Opened module path=%s", src->buf);
  profile_func_end;
  return module_value;
}

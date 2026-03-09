// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/entry.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "memory/vmem.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <olib.h>
#include <stdlib.h>
#include <string.h>

// =========================================================================
// Entry runtime state
// =========================================================================

typedef struct entry_shared_state {
  sz sdl_init_depth;
  b32 owns_sdl;
  sz global_ctx_init_depth;
  b32 owns_global_ctx;
} entry_shared_state;

typedef struct entry_thread_state {
  sz thread_ctx_init_depth;
  b32 owns_thread_ctx;
} entry_thread_state;

global_var entry_shared_state entry_shared = {0};
thread_local global_var entry_thread_state entry_thread = {0};
global_var allocator entry_memory_fallback = {0};
global_var cmdline entry_cmdline_current = {0};
global_var allocator entry_start_allocator = {0};

func allocator entry_get_pipeline_allocator(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_get_allocator();
  if (!alloc.alloc_fn) {
    alloc = global_get_allocator();
  }
  if (!alloc.alloc_fn) {
    alloc = entry_memory_fallback;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return alloc;
}

func void* entry_pipeline_malloc(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = entry_get_pipeline_allocator();
  void* ptr = _allocator_alloc(&alloc, size, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func void entry_pipeline_free(void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  allocator alloc = entry_get_pipeline_allocator();
  _allocator_dealloc(&alloc, ptr, 0, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* entry_pipeline_calloc(sz count, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = entry_get_pipeline_allocator();
  void* ptr = _allocator_calloc(&alloc, count, size, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func void* entry_pipeline_realloc(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = entry_get_pipeline_allocator();
  void* new_ptr = _allocator_realloc(&alloc, ptr, 0, size, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return new_ptr;
}

func cmdline entry_get_cmdline(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return entry_cmdline_current;
}

func allocator entry_default_allocator(allocator start_alloc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = start_alloc;
  if (!alloc.alloc_fn) {
#ifdef OVERRIDE_GLOBAL_DEFAULT_ALLOCATOR
    alloc = global_allocator_default();
#else
    alloc = vmem_get_allocator();
#endif
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return alloc;
}

// =========================================================================
// Common lifecycle hooks
// =========================================================================

func b32 entry_init(cmdline cmdline) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (entry_cmdline_current.args == NULL && cmdline.args != NULL && cmdline.count > 0) {
    entry_cmdline_current = cmdline;
  }

  allocator default_global_allocator = entry_default_allocator(entry_start_allocator);
  entry_memory_fallback = default_global_allocator;

  olib_set_memory_fns(
      entry_pipeline_malloc,
      entry_pipeline_free,
      entry_pipeline_calloc,
      entry_pipeline_realloc);

  if (!SDL_SetMemoryFunctions(
          entry_pipeline_malloc,
          entry_pipeline_calloc,
          entry_pipeline_realloc,
          entry_pipeline_free)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  SDL_InitFlags init_flags =
      SDL_INIT_AUDIO |
      SDL_INIT_VIDEO |
      SDL_INIT_JOYSTICK |
      SDL_INIT_HAPTIC |
      SDL_INIT_GAMEPAD |
      SDL_INIT_EVENTS |
      SDL_INIT_SENSOR |
      SDL_INIT_CAMERA;

  if (entry_shared.sdl_init_depth == 0) {
    if (!SDL_WasInit(init_flags)) {
      if (!SDL_Init(init_flags)) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }
      thread_log_trace("entry_init: SDL initialized");
      entry_shared.owns_sdl = true;
    } else {
      entry_shared.owns_sdl = false;
    }
  }
  entry_shared.sdl_init_depth += 1;

  if (entry_shared.global_ctx_init_depth == 0) {
    if (!global_ctx_is_init()) {
      if (!global_ctx_init(default_global_allocator)) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }

      entry_shared.owns_global_ctx = true;
      thread_log_trace("entry_init: global_ctx initialized");
    } else {
      entry_shared.owns_global_ctx = false;
    }
  }
  entry_shared.global_ctx_init_depth += 1;

  if (entry_thread.thread_ctx_init_depth == 0) {
    if (!thread_ctx_is_init()) {
      if (!thread_ctx_init(global_get_allocator())) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }

      entry_thread.owns_thread_ctx = true;
      (void)thread_log_sync();
      thread_log_trace("entry_init: thread_ctx initialized");
    } else {
      entry_thread.owns_thread_ctx = false;
    }
  }
  entry_thread.thread_ctx_init_depth += 1;

  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void entry_quit(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (entry_thread.thread_ctx_init_depth > 0) {
    entry_thread.thread_ctx_init_depth -= 1;
    if (entry_thread.thread_ctx_init_depth == 0 && entry_thread.owns_thread_ctx) {
      thread_log_trace("entry_quit: thread_ctx");
      thread_ctx_quit();
      entry_thread.owns_thread_ctx = false;
    }
  }

  if (entry_shared.global_ctx_init_depth > 0) {
    entry_shared.global_ctx_init_depth -= 1;
    if (entry_shared.global_ctx_init_depth == 0 && entry_shared.owns_global_ctx) {
      thread_log_trace("entry_quit: global_ctx");
      global_ctx_quit();
      entry_shared.owns_global_ctx = false;
    }
  }

  if (entry_shared.sdl_init_depth > 0) {
    entry_shared.sdl_init_depth -= 1;
    if (entry_shared.sdl_init_depth == 0 && entry_shared.owns_sdl) {
      thread_log_trace("entry_quit: SDL");
      SDL_Quit();
      entry_shared.owns_sdl = false;
    }
  }
  if (entry_shared.sdl_init_depth == 0 && entry_shared.global_ctx_init_depth == 0 &&
      entry_thread.thread_ctx_init_depth == 0) {
    entry_cmdline_current = (cmdline) {0};
    entry_start_allocator = (allocator) {0};
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Application entry point
// =========================================================================

global_var b32 app_entry_initialized = false;
global_var b32 app_callbacks_started = false;
global_var entry_app_init_fn* app_init_callback = NULL;
global_var entry_app_update_fn* app_update_callback = NULL;
global_var entry_app_quit_fn* app_quit_callback = NULL;
global_var entry_run_fn* run_callback = NULL;

func void entry_set_app_callbacks(
    entry_app_init_fn* init_fn,
    entry_app_update_fn* update_fn,
    entry_app_quit_fn* quit_fn) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  app_init_callback = init_fn;
  app_update_callback = update_fn;
  app_quit_callback = quit_fn;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void entry_set_run_callback(entry_run_fn* callback_fn) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  run_callback = callback_fn;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func SDL_AppResult app_result_to_sdl_result(app_result result) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  switch (result) {
    case APP_RESULT_CONTINUE:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_CONTINUE;
    case APP_RESULT_SUCCESS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_SUCCESS;
    case APP_RESULT_FAIL:
    case APP_RESULT_MAX:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_FAILURE;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  cmdline cmdl = cmdline_build((sz)argc, (c8**)argv);

  if (appstate != NULL) {
    *appstate = NULL;
  }

  app_entry_initialized = false;
  app_callbacks_started = false;

  if (!entry_init(cmdl)) {
    return SDL_APP_FAILURE;
  }

  if (app_init_callback == NULL || app_update_callback == NULL || app_quit_callback == NULL) {
    entry_quit();
    return SDL_APP_FAILURE;
  }

  app_entry_initialized = true;

  app_callbacks_started = true;
  return app_result_to_sdl_result(app_init_callback(cmdl, appstate));
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  (void)result;

  if (app_callbacks_started && app_quit_callback != NULL) {
    app_quit_callback(appstate);
    app_callbacks_started = false;
  }

  if (app_entry_initialized) {
    entry_quit();
    app_entry_initialized = false;
  }
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  if (app_update_callback == NULL) {
    return SDL_APP_FAILURE;
  }

  return app_result_to_sdl_result(app_update_callback(appstate));
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  msg based_msg = {0};
  (void)appstate;
  if (event->type == SDL_EVENT_QUIT) {
    if (msg_from_native(event, &based_msg)) {
      (void)msg_post(&based_msg);
    }
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

int main_app(int argc, char** argv, allocator alloc) {
  entry_start_allocator = alloc;
  return SDL_EnterAppMainCallbacks(argc, argv, SDL_AppInit, SDL_AppIterate, SDL_AppEvent, SDL_AppQuit);
}

// =========================================================================
// Run entry point
// =========================================================================

int main_run_internal(int argc, char** argv) {
  cmdline cmdl = cmdline_build((sz)argc, (c8**)argv);
  if (!entry_init(cmdl)) {
    return EXIT_FAILURE;
  }

  if (run_callback == NULL) {
    entry_quit();
    return EXIT_FAILURE;
  }

  b32 result = run_callback(cmdl);
  entry_quit();

  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main_run(int argc, char** argv, allocator alloc) {
  entry_start_allocator = alloc;
  return SDL_RunApp(argc, argv, main_run_internal, NULL);
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/log.h"
#include "threads/atomics.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// =========================================================================
// Internal state
// =========================================================================

global_var log_state global_log_state = {0};
global_var atomic_i32 global_log_state_init = {0};

global_var mutex log_emit_mutex = NULL;
global_var atomic_i32 log_emit_mutex_init = {0};

// =========================================================================
// Internal helpers
// =========================================================================

func mutex log_shared_mutex_get(atomic_i32* init_state, mutex* out_mutex) {
  if (atomic_i32_get(init_state) == 2) {
    return *out_mutex;
  }

  i32 expected = 0;
  if (atomic_i32_cmpex(init_state, &expected, 1)) {
    *out_mutex = mutex_create();
    atomic_fence_release();
    atomic_i32_set(init_state, 2);
    return *out_mutex;
  }

  while (atomic_i32_get(init_state) != 2) {
    atomic_pause();
  }
  atomic_fence_acquire();
  return *out_mutex;
}

func log_state* log_state_resolve(log_state* state) {
  if (state) {
    return state->is_initialized ? state : NULL;
  }
  return log_get_global_state();
}

func void log_state_lock(log_state* state) {
  if (state && state->mutex_handle) {
    mutex_lock(state->mutex_handle);
  }
}

func void log_state_unlock(log_state* state) {
  if (state && state->mutex_handle) {
    mutex_unlock(state->mutex_handle);
  }
}

// Returns the ANSI foreground-color escape sequence for the given log level.
func const c8* log_level_to_color(log_level level) {
  switch (level) {
    case LOG_LEVEL_FATAL:
      return "\033[1;31m";  // bold red
    case LOG_LEVEL_ERROR:
      return "\033[0;31m";  // red
    case LOG_LEVEL_WARN:
      return "\033[0;33m";  // yellow
    case LOG_LEVEL_INFO:
      return "\033[0;32m";  // green
    case LOG_LEVEL_DEBUG:
      return "\033[0;36m";  // cyan
    case LOG_LEVEL_VERBOSE:
      return "\033[0;34m";  // blue
    case LOG_LEVEL_TRACE:
      return "\033[0;37m";  // gray
    case LOG_LEVEL_MAX:
      return "\033[0m";
  }
  return "\033[0m";
}

// Emits a single formatted log line to stderr.
func void log_emit(log_level level, callsite site, const c8* msg) {
  mutex emit_lock = log_shared_mutex_get(&log_emit_mutex_init, &log_emit_mutex);
  if (emit_lock) {
    mutex_lock(emit_lock);
  }

  const c8* color = log_level_to_color(level);
  const c8* label = log_level_to_str(level);
  fprintf(stderr, "%s[%s]\033[0m %s  \033[0;90m(%s() %s:%u)\033[0m\n", color, label, msg, site.function, site.filename, site.line);
  fflush(stderr);

  if (emit_lock) {
    mutex_unlock(emit_lock);
  }
}

// =========================================================================
// Public API
// =========================================================================

func const c8* log_level_to_str(log_level level) {
  switch (level) {
    case LOG_LEVEL_FATAL:
      return "FATAL";
    case LOG_LEVEL_ERROR:
      return "ERROR";
    case LOG_LEVEL_WARN:
      return "WARN ";
    case LOG_LEVEL_INFO:
      return "INFO ";
    case LOG_LEVEL_DEBUG:
      return "DEBUG";
    case LOG_LEVEL_VERBOSE:
      return "VERB ";
    case LOG_LEVEL_TRACE:
      return "TRACE";
    case LOG_LEVEL_MAX:
      return "?????";
  }
  return NULL;
}

func b32 log_state_init(log_state* state, b32 use_mutex) {
  if (!state) {
    return false;
  }

  memset(state, 0, sizeof(*state));
  state->level = LOG_LEVEL_DEFAULT;
  state->callback = NULL;
  if (use_mutex) {
    state->mutex_handle = mutex_create();
    if (!state->mutex_handle) {
      return false;
    }
  }
  state->is_initialized = true;
  return true;
}

func void log_state_quit(log_state* state) {
  if (!state) {
    return;
  }

  mutex state_mutex = state->mutex_handle;
  memset(state, 0, sizeof(*state));
  if (state_mutex) {
    mutex_destroy(state_mutex);
  }
}

func b32 log_state_is_initialized(log_state* state) {
  return state != NULL && state->is_initialized;
}

func void log_state_set_level(log_state* state, log_level level) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }

  log_state_lock(resolved);
  resolved->level = level;
  log_state_unlock(resolved);
}

func void log_state_set_callback(log_state* state, log_callback callback) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }

  log_state_lock(resolved);
  resolved->callback = callback;
  log_state_unlock(resolved);
}

func void log_state_sync(log_state* dst, log_state* src) {
  log_state* resolved_dst = log_state_resolve(dst);
  log_state* resolved_src = log_state_resolve(src);
  if (!resolved_dst || !resolved_src) {
    return;
  }

  if (resolved_dst == resolved_src || !resolved_dst->is_initialized || !resolved_src->is_initialized) {
    return;
  }

  log_level level = LOG_LEVEL_DEFAULT;
  log_callback callback = NULL;

  log_state_lock(resolved_src);
  level = resolved_src->level;
  callback = resolved_src->callback;
  log_state_unlock(resolved_src);

  log_state_lock(resolved_dst);
  resolved_dst->level = level;
  resolved_dst->callback = callback;
  log_state_unlock(resolved_dst);
}

func log_state* log_get_global_state(void) {
  if (atomic_i32_get(&global_log_state_init) != 2) {
    i32 expected = 0;
    if (atomic_i32_cmpex(&global_log_state_init, &expected, 1)) {
      log_state_init(&global_log_state, true);
      atomic_fence_release();
      atomic_i32_set(&global_log_state_init, 2);
    } else {
      while (atomic_i32_get(&global_log_state_init) != 2) {
        atomic_pause();
      }
      atomic_fence_acquire();
    }
  }
  return &global_log_state;
}

func void _log(log_state* state, log_level level, callsite site, const c8* msg, ...) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }

  log_level active_level = LOG_LEVEL_DEFAULT;
  log_callback callback = NULL;

  log_state_lock(resolved);
  active_level = resolved->level;
  callback = resolved->callback;
  log_state_unlock(resolved);

  if (level > active_level) {
    return;
  }

  c8 buf[1024];
  va_list args;
  va_start(args, msg);
  vsnprintf(buf, sizeof(buf), msg, args);
  va_end(args);

  if (callback != NULL) {
    b32 should_continue = callback(level, buf, site);
    if (!should_continue) {
      return;
    }
  }

  log_emit(level, site, buf);
}

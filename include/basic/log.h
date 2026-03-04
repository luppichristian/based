// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "codespace.h"
#include "primitive_types.h"
#include "../threads/mutex.h"

// =========================================================================
// Log function
// =========================================================================

// Log levels for logging messages.
// These levels can be used to filter logs based on their severity.
typedef enum log_level {
  LOG_LEVEL_FATAL,    // A fatal error that causes the application to trigger an assert.
  LOG_LEVEL_ERROR,    // An error that should be fixed, but does not cause an assert.
  LOG_LEVEL_WARN,     // A warning that should be fixed, but does not cause an assert.
  LOG_LEVEL_INFO,     // Info message that does not indicate a problem.
  LOG_LEVEL_DEBUG,    // Debug message that is only logged in debug builds.
  LOG_LEVEL_VERBOSE,  // Verbose message that is only logged in debug builds and is more detailed.
  LOG_LEVEL_TRACE,    // Trace message that is only logged in debug builds and is very detailed, used for tracing.
  LOG_LEVEL_MAX,
} log_level;

// Function prototype for the log callback.
// This callback can be set by the user to handle logs in a custom way.
// If 'true' is returned, the default log handling will be called,
// otherwise it will be ignored.
typedef b32 (*log_callback)(log_level level, const char* msg, callsite site);

// Per-thread (or caller-owned) log configuration.
// The mutex is optional. When present, it serializes access to callback/level.
typedef struct log_state {
  b32 is_initialized;
  mutex mutex_handle;
  log_level level;
  log_callback callback;
} log_state;

// Returns label string for the given log level.
func const c8* log_level_to_str(log_level level);

// Initializes a log state with LOG_LEVEL_DEFAULT and no callback.
// When use_mutex is true the state creates and owns a mutex; otherwise it is lock-free.
func b32 log_state_init(log_state* state, b32 use_mutex);

// Releases resources owned by the log state and zeroes it.
// Safe to call on NULL or zero-initialized states.
func void log_state_quit(log_state* state);

// Returns true if the state is non-NULL and has been initialized.
func b32 log_state_is_initialized(log_state* state);

// Sets the minimum enabled level for the given state.
func void log_state_set_level(log_state* state, log_level level);

// Sets the callback for the given state.
func void log_state_set_callback(log_state* state, log_callback callback);

// Copies the effective level/callback from src into dst.
// This does not create or destroy mutexes; each state keeps its own locking mode.
func void log_state_sync(log_state* dst, log_state* src);

// Returns the process-global fallback log state.
// It is created lazily on first use and is safe to use from any thread.
func log_state* log_get_global_state(void);

// Log function called by the logging macros.
// Passing NULL falls back to log_get_global_state().
func void _log(log_state* state, log_level level, callsite site, const char* msg, ...);

// Convenience macros for logging with an explicit state.
#define log_state_fatal(state, ...)   _log((state), LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define log_state_error(state, ...)   _log((state), LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define log_state_warn(state, ...)    _log((state), LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define log_state_info(state, ...)    _log((state), LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define log_state_debug(state, ...)   _log((state), LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define log_state_verbose(state, ...) _log((state), LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define log_state_trace(state, ...)   _log((state), LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// Convenience macros for logging to the process-global main log state.
#define log_fatal(...)   _log(NULL, LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define log_error(...)   _log(NULL, LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define log_warn(...)    _log(NULL, LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define log_info(...)    _log(NULL, LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define log_debug(...)   _log(NULL, LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define log_verbose(...) _log(NULL, LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define log_trace(...)   _log(NULL, LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// Define default log level if not defined by the user.
#ifndef LOG_LEVEL_DEFAULT
#  ifdef BUILD_DEBUG
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#  else
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO
#  endif
#endif

// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "codespace.h"
#include "primitive_types.h"

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

// Returns label string for the given log level.
func const c8* log_level_to_str(log_level level);

// Log function called by the LOG macro.
func void _log(log_level level, callsite site, const char* msg, ...);

// Convenience macros for logging. They will call the _log function with the appropriate log level.
#define log_fatal(...)   _log(LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define log_error(...)   _log(LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define log_warn(...)    _log(LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define log_info(...)    _log(LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define log_debug(...)   _log(LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define log_verbose(...) _log(LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define log_trace(...)   _log(LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// =========================================================================
// Log callback
// =========================================================================

// Function prototype for the log callback.
// This callback can be set by the user to handle logs in a custom way.
// If 'true' is returned, the default log handling will be called,
// otherwise it will be ignored.
typedef b32 (*log_callback)(log_level level, const char* msg, callsite site);

// Set the log callback function. This allows the user to handle logs in a custom way.
func void log_set_callback(log_callback callback);

// =========================================================================
// Global log level
// =========================================================================

// Define default log level if not defined by the user.
#ifndef LOG_LEVEL_DEFAULT
#  ifdef BUILD_DEBUG
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#  else
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO
#  endif
#endif

// Set the global log level. Logs with a level below this will be ignored.
func void log_set_level(log_level level);
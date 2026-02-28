// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/log.h"

#include <stdarg.h>
#include <stdio.h>

// =========================================================================
// Internal state
// =========================================================================

global_var log_level log_level_current = LOG_LEVEL_DEFAULT;
global_var log_callback log_callback_current = NULL;

// =========================================================================
// Internal helpers
// =========================================================================

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
  const c8* color = log_level_to_color(level);
  const c8* label = log_level_to_str(level);
  fprintf(stderr, "%s[%s]\033[0m %s  \033[0;90m(%s() %s:%u)\033[0m\n", color, label, msg, site.function, site.filename, site.line);
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

func void log_set_level(log_level level) {
  log_level_current = level;
}

func void log_set_callback(log_callback callback) {
  log_callback_current = callback;
}

func void _log(log_level level, callsite site, const c8* msg, ...) {
  if (level > log_level_current) {
    return;
  }

  c8 buf[1024];
  va_list args;
  va_start(args, msg);
  vsnprintf(buf, sizeof(buf), msg, args);
  va_end(args);

  if (log_callback_current != NULL) {
    b32 should_continue = log_callback_current(level, buf, site);
    if (!should_continue) {
      return;
    }
  }

  log_emit(level, site, buf);
}

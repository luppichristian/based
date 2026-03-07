// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "env_defines.h"
#include "primitive_types.h"

// =========================================================================
// Profiler configuration
// =========================================================================

// Enable profiling by default in debug/profile builds.
// Users can override this with -DBASED_PROFILER_ENABLED=0/1.
#ifndef BASED_PROFILER_ENABLED
#  if defined(BUILD_DEBUG) || defined(BUILD_PROFILE)
#    define BASED_PROFILER_ENABLED 1
#  else
#    define BASED_PROFILER_ENABLED 0
#  endif
#endif

// =========================================================================
// Tracy include
// =========================================================================

// If TRACY_ENABLE is undefined, tracy will replace all function calls with stubs.
#if BASED_PROFILER_ENABLED
#  define TRACY_ENABLE
#else
#  undef TRACY_ENABLE
#endif

// Set max callstack depth
#define TRACY_CALLSTACK 32

#include <tracy/TracyC.h>
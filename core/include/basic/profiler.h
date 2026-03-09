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

// Respect build-system definitions and only enable Tracy when profiling is on.
#if BASED_PROFILER_ENABLED
#  if !defined(TRACY_ENABLE)
#    define TRACY_ENABLE 1
#  endif
#endif

// Set max callstack depth
#define TRACY_CALLSTACK 32

// Include Tracy profiler, macros expand to nothing if tracy is disabled.
#include <tracy/TracyC.h>

// Helper functions for convenience
#define profile_func_begin TracyCZoneN(__tracy_zone_ctx, __func__, 1)
#define profile_func_end   TracyCZoneEnd(__tracy_zone_ctx)
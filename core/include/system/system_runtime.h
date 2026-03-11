// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct system_runtime_info {
  // Approximate total CPU busy percentage since the previous query.
  // The first sample is usually 0 because no prior baseline exists yet.
  f32 cpu_usage_percent;

  // Total physical memory visible to the host operating system.
  sz memory_total;

  // Derived host memory currently considered in use.
  sz memory_used;

  // Derived host memory currently available for allocation without reclaim pressure.
  sz memory_available;

  // Resident memory currently used by this process.
  sz process_memory_used;

  // Peak resident memory observed for this process when the platform exposes it.
  sz process_memory_peak;
} system_runtime_info;

// Queries current host and process runtime metrics.
// cpu_usage_percent is measured as a delta from the previous call and may be 0 on the first sample.
func b32 system_runtime_query(system_runtime_info* out_info);

// =========================================================================
c_end;
// =========================================================================

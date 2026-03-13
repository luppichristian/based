// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "monitor.h"
#include "window.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct dpi_metrics {
  f32 content_scale;
  f32 pixel_density;
  f32 display_scale;
} dpi_metrics;

func dpi_metrics dpi_metrics_make_default(void);

// Writes DPI metrics for monitor_id into out_metrics. Returns 1 on success, 0 otherwise.
func b32 dpi_get_monitor_metrics(monitor monitor_id, dpi_metrics* out_metrics);

// Writes DPI metrics for window_id into out_metrics. Returns 1 on success, 0 otherwise.
func b32 dpi_get_window_metrics(window window_id, dpi_metrics* out_metrics);

// Scales value by scale and returns the floating-point result.
func f32 dpi_scale_f32(f32 value, f32 scale);

// Scales value by scale and returns the integer result.
func i32 dpi_scale_i32(i32 value, f32 scale);

// =========================================================================
c_end;
// =========================================================================

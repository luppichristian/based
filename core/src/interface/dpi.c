// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/dpi.h"
#include "context/thread_ctx.h"
#include "../internal.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func dpi_metrics dpi_metrics_make_default(void) {
  dpi_metrics result = {0};
  result.content_scale = 1.0f;
  result.pixel_density = 1.0f;
  result.display_scale = 1.0f;
  return result;
}

func b32 dpi_get_monitor_metrics(monitor id, dpi_metrics* out_metrics) {
  profile_func_begin;
  if (out_metrics == NULL || !monitor_id_is_valid(id)) {
    profile_func_end;
    return false;
  }

  *out_metrics = dpi_metrics_make_default();
  out_metrics->content_scale = monitor_get_content_scale(id);
  out_metrics->display_scale = out_metrics->content_scale;
  profile_func_end;
  return true;
}

func b32 dpi_get_window_metrics(window id, dpi_metrics* out_metrics) {
  profile_func_begin;
  if (out_metrics == NULL || !window_id_is_valid(id)) {
    profile_func_end;
    return false;
  }

  SDL_Window* window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  if (window_ptr == NULL) {
    thread_log_error("Failed to query DPI metrics for invalid window id=%llu", (unsigned long long)window_to_native_id(id));
    profile_func_end;
    return false;
  }

  *out_metrics = dpi_metrics_make_default();
  out_metrics->pixel_density = SDL_GetWindowPixelDensity(window_ptr);
  out_metrics->display_scale = SDL_GetWindowDisplayScale(window_ptr);

  monitor monitor_id = NULL;
  if (window_get_monitor(id, &monitor_id)) {
    out_metrics->content_scale = monitor_get_content_scale(monitor_id);
  }

  if (out_metrics->pixel_density <= 0.0f) {
    out_metrics->pixel_density = 1.0f;
  }
  if (out_metrics->display_scale <= 0.0f) {
    out_metrics->display_scale = out_metrics->content_scale > 0.0f ? out_metrics->content_scale : 1.0f;
  }
  if (out_metrics->content_scale <= 0.0f) {
    out_metrics->content_scale = out_metrics->display_scale;
  }

  profile_func_end;
  return true;
}

func f32 dpi_scale_f32(f32 value, f32 scale) {
  return value * scale;
}

func i32 dpi_scale_i32(i32 value, f32 scale) {
  return (i32)((f32)value * scale);
}

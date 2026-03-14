// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/monitor.h"
#include "../internal.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// =========================================================================
// Identifier Conversion Helpers
// =========================================================================

func b32 monitor_id_is_valid(monitor src) {
  return src != NULL;
}

func monitor monitor_from_device(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_MONITOR) {
    return NULL;
  }

  return (monitor)(up)devices_get_instance(src);
}

func monitor monitor_from_native_id(up native_id) {
  return (monitor)(up)native_id;
}

func up monitor_to_native_id(monitor src) {
  return (up)src;
}

// =========================================================================
// Global Monitor Enumeration
// =========================================================================

func sz monitor_get_total_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);

  if (ids == NULL && count != 0) {
    thread_log_warn("Failed to enumerate monitors error=%s", SDL_GetError());
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func monitor monitor_get_from_idx(sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);
  b32 found = ids != NULL && idx < (sz)count;
  monitor out_monitor = NULL;
  if (found) {
    out_monitor = monitor_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return out_monitor;
}

func monitor monitor_get_primary_id(void) {
  profile_func_begin;
  monitor result = monitor_from_native_id((up)SDL_GetPrimaryDisplay());
  if (!monitor_id_is_valid(result)) {
    thread_log_warn("Primary monitor is unavailable");
  }
  profile_func_end;
  return result;
}

func cstr8 monitor_get_name(monitor id) {
  if (!monitor_id_is_valid(id)) {
    return NULL;
  }

  return SDL_GetDisplayName((SDL_DisplayID)monitor_to_native_id(id));
}

// =========================================================================
// Monitor Geometry
// =========================================================================

func r2_i32 monitor_get_bounds(monitor id) {
  r2_i32 out_rect = {0};
  if (!monitor_id_is_valid(id)) {
    return out_rect;
  }
  profile_func_begin;

  SDL_Rect bounds = {0};

  if (!monitor_id_is_valid(id) ||
      !SDL_GetDisplayBounds((SDL_DisplayID)monitor_to_native_id(id), &bounds)) {
    thread_log_error("Failed to query monitor bounds id=%llu error=%s",
                     (unsigned long long)monitor_to_native_id(id),
                     SDL_GetError());
    profile_func_end;
    return out_rect;
  }

  out_rect.min.x = bounds.x;
  out_rect.min.y = bounds.y;
  out_rect.max.x = bounds.x + bounds.w;
  out_rect.max.y = bounds.y + bounds.h;
  profile_func_end;
  return out_rect;
}

func r2_i32 monitor_get_usable_bounds(monitor id) {
  r2_i32 out_rect = {0};
  if (!monitor_id_is_valid(id)) {
    return out_rect;
  }
  profile_func_begin;

  SDL_Rect bounds = {0};
  if (!SDL_GetDisplayUsableBounds((SDL_DisplayID)monitor_to_native_id(id), &bounds)) {
    thread_log_error("Failed to query monitor usable bounds id=%llu error=%s",
                     (unsigned long long)monitor_to_native_id(id),
                     SDL_GetError());
    profile_func_end;
    return out_rect;
  }

  out_rect.min.x = bounds.x;
  out_rect.min.y = bounds.y;
  out_rect.max.x = bounds.x + bounds.w;
  out_rect.max.y = bounds.y + bounds.h;
  profile_func_end;
  return out_rect;
}

// =========================================================================
// Display Mode Enumeration
// =========================================================================

func b32 monitor_mode_from_native(const SDL_DisplayMode* native_mode, monitor_mode* out_mode) {
  profile_func_begin;
  if (native_mode == NULL || out_mode == NULL) {
    profile_func_end;
    return false;
  }

  out_mode->width = native_mode->w;
  out_mode->height = native_mode->h;
  out_mode->pixel_format = native_mode->format;
  out_mode->refresh_rate = native_mode->refresh_rate;
  profile_func_end;
  return true;
}

func sz monitor_get_mode_count(monitor id) {
  profile_func_begin;
  if (!monitor_id_is_valid(id)) {
    thread_log_warn("Rejected monitor mode count query for invalid monitor");
    profile_func_end;
    return 0;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)monitor_to_native_id(id), &mode_count);
  if (modes != NULL) {
    SDL_free(modes);
  }

  profile_func_end;
  return mode_count > 0 ? (sz)mode_count : 0;
}

func b32 monitor_get_mode(monitor id, sz idx, monitor_mode* out_mode) {
  profile_func_begin;
  if (out_mode == NULL) {
    profile_func_end;
    return false;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)monitor_to_native_id(id), &mode_count);
  b32 ok = modes != NULL && idx < (sz)mode_count;

  if (!ok) {
    thread_log_error("Failed to query monitor mode id=%llu idx=%zu count=%d",
                     (unsigned long long)monitor_to_native_id(id),
                     (size_t)idx,
                     mode_count);
    if (modes != NULL) {
      SDL_free(modes);
    }
    profile_func_end;
    return false;
  }

  b32 result = monitor_mode_from_native(modes[idx], out_mode);
  SDL_free(modes);
  profile_func_end;
  return result;
}

func b32 monitor_get_current_mode(monitor id, monitor_mode* out_mode) {
  profile_func_begin;
  if (!monitor_id_is_valid(id) || out_mode == NULL) {
    thread_log_error("Rejected current monitor mode query id=%llu out_mode=%p",
                     (unsigned long long)monitor_to_native_id(id),
                     (void*)out_mode);
    profile_func_end;
    return false;
  }

  const SDL_DisplayMode* native_mode = SDL_GetCurrentDisplayMode((SDL_DisplayID)monitor_to_native_id(id));
  b32 result = monitor_mode_from_native(native_mode, out_mode);
  profile_func_end;
  return result;
}

func b32 monitor_get_desktop_mode(monitor id, monitor_mode* out_mode) {
  profile_func_begin;
  if (!monitor_id_is_valid(id) || out_mode == NULL) {
    thread_log_error("Rejected desktop monitor mode query id=%llu out_mode=%p",
                     (unsigned long long)monitor_to_native_id(id),
                     (void*)out_mode);
    profile_func_end;
    return false;
  }

  const SDL_DisplayMode* native_mode = SDL_GetDesktopDisplayMode((SDL_DisplayID)monitor_to_native_id(id));
  b32 result = monitor_mode_from_native(native_mode, out_mode);
  profile_func_end;
  return result;
}

// =========================================================================
// Convenience Queries
// =========================================================================

func f32 monitor_get_refresh_rate(monitor id) {
  profile_func_begin;
  monitor_mode mode = {0};
  if (!monitor_get_current_mode(id, &mode)) {
    profile_func_end;
    return 0.0f;
  }

  profile_func_end;
  return mode.refresh_rate;
}

func f32 monitor_get_content_scale(monitor id) {
  profile_func_begin;
  if (!monitor_id_is_valid(id)) {
    profile_func_end;
    return 1.0f;
  }

  f32 scale = SDL_GetDisplayContentScale((SDL_DisplayID)monitor_to_native_id(id));
  if (scale <= 0.0f) {
    profile_func_end;
    return 1.0f;
  }

  profile_func_end;
  return scale;
}

func monitor_orientation monitor_get_orientation(monitor id) {
  profile_func_begin;
  if (!monitor_id_is_valid(id)) {
    profile_func_end;
    return MONITOR_ORIENTATION_UNKNOWN;
  }

  SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation((SDL_DisplayID)monitor_to_native_id(id));
  switch (orientation) {
    case SDL_ORIENTATION_LANDSCAPE:
      profile_func_end;
      return MONITOR_ORIENTATION_LANDSCAPE;
    case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
      profile_func_end;
      return MONITOR_ORIENTATION_LANDSCAPE_FLIPPED;
    case SDL_ORIENTATION_PORTRAIT:
      profile_func_end;
      return MONITOR_ORIENTATION_PORTRAIT;
    case SDL_ORIENTATION_PORTRAIT_FLIPPED:
      profile_func_end;
      return MONITOR_ORIENTATION_PORTRAIT_FLIPPED;
    case SDL_ORIENTATION_UNKNOWN:
    default:
      profile_func_end;
      return MONITOR_ORIENTATION_UNKNOWN;
  }
}

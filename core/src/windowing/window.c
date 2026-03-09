// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/window.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// =========================================================================
// Internal Helpers
// =========================================================================

func SDL_Window* window_resolve(window id) {
  profile_func_begin;
  if (!window_id_is_valid(id)) {
    profile_func_end;
    return NULL;
  }
  SDL_Window* window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  profile_func_end;
  return window_ptr;
}

// =========================================================================
// Identifier Conversion Helpers
// =========================================================================

func b32 window_id_is_valid(window src) {
  profile_func_begin;
  profile_func_end;
  return src != NULL;
}

func window window_from_native_id(up native_id) {
  profile_func_begin;
  profile_func_end;
  return (window)(up)native_id;
}

func up window_to_native_id(window src) {
  profile_func_begin;
  profile_func_end;
  return (up)src;
}

// =========================================================================
// Global Window Enumeration
// =========================================================================

func sz window_get_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);

  if (windows) {
    SDL_free(windows);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func b32 window_get_id(sz idx, window* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);
  b32 found = windows != NULL && idx < (sz)count && windows[idx] != NULL;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = window_from_native_id((up)SDL_GetWindowID(windows[idx]));
  }

  if (windows) {
    SDL_free(windows);
  }

  profile_func_end;
  return found;
}

// =========================================================================
// Creation and Destruction
// =========================================================================

func b32 window_is_valid(window id) {
  profile_func_begin;
  profile_func_end;
  return window_resolve(id) != NULL;
}

func window window_create(cstr8 title, i32 width, i32 height, u64 flags) {
  profile_func_begin;
  SDL_Window* window_ptr = SDL_CreateWindow(title != NULL ? title : "", width, height, (SDL_WindowFlags)flags);
  if (window_ptr == NULL) {
    profile_func_end;
    return NULL;
  }

  window result = window_from_native_id((up)SDL_GetWindowID(window_ptr));
  profile_func_end;
  return result;
}

func b32 window_destroy(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_DestroyWindow(window_ptr);
  profile_func_end;
  return 1;
}

// =========================================================================
// Visibility
// =========================================================================

func b32 window_show(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_ShowWindow(window_ptr);
  profile_func_end;
  return 1;
}

func b32 window_hide(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_HideWindow(window_ptr);
  profile_func_end;
  return 1;
}

// =========================================================================
// Position
// =========================================================================

func b32 window_move(window id, i32 xpos, i32 ypos) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_SetWindowPosition(window_ptr, xpos, ypos);
  profile_func_end;
  return 1;
}

func b32 window_get_position(window id, i32* out_xpos, i32* out_ypos) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || out_xpos == NULL || out_ypos == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_GetWindowPosition(window_ptr, out_xpos, out_ypos);
  profile_func_end;
  return 1;
}

// =========================================================================
// Size
// =========================================================================

func b32 window_resize(window id, i32 width, i32 height) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_SetWindowSize(window_ptr, width, height);
  profile_func_end;
  return 1;
}

func b32 window_get_size(window id, i32* out_width, i32* out_height) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || out_width == NULL || out_height == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_GetWindowSize(window_ptr, out_width, out_height);
  profile_func_end;
  return 1;
}

// =========================================================================
// Fullscreen State
// =========================================================================

func b32 window_set_fullscreen(window id, b32 enabled) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  b32 result = SDL_SetWindowFullscreen(window_ptr, enabled != 0);
  profile_func_end;
  return result;
}

func b32 window_is_fullscreen(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  b32 result = (SDL_GetWindowFlags(window_ptr) & SDL_WINDOW_FULLSCREEN) != 0 ? 1 : 0;
  profile_func_end;
  return result;
}

// =========================================================================
// Window-State Controls
// =========================================================================

func b32 window_minimize(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_MinimizeWindow(window_ptr);
  profile_func_end;
  return 1;
}

func b32 window_maximize(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_MaximizeWindow(window_ptr);
  profile_func_end;
  return 1;
}

func b32 window_restore(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_RestoreWindow(window_ptr);
  profile_func_end;
  return 1;
}

func b32 window_focus(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_RaiseWindow(window_ptr);
  profile_func_end;
  return 1;
}

// =========================================================================
// Metadata
// =========================================================================

func cstr8 window_get_title(window id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return SDL_GetWindowTitle(window_ptr);
}

func b32 window_set_title(window id, cstr8 title) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || title == NULL) {
    profile_func_end;
    return 0;
  }

  SDL_SetWindowTitle(window_ptr, title);
  profile_func_end;
  return 1;
}

func b32 window_get_monitor_id(window id, monitor* out_monitor_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(id);
  SDL_DisplayID native_monitor_id = 0;

  if (out_monitor_id) {
    *out_monitor_id = NULL;
  }

  if (window_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  native_monitor_id = SDL_GetDisplayForWindow(window_ptr);
  if (native_monitor_id == 0) {
    profile_func_end;
    return 0;
  }

  if (out_monitor_id) {
    *out_monitor_id = monitor_from_native_id((up)native_monitor_id);
  }

  profile_func_end;
  return 1;
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/cursor.h"
#include "context/thread_ctx.h"
#include "../internal.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "icon_internal.h"

global_var SDL_Cursor* cursor_active_handle = NULL;

func SDL_Window* cursor_resolve_window(window opt_window) {
  if (window_id_is_valid(opt_window)) {
    return SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }

  return SDL_GetMouseFocus();
}

func void cursor_release_active_handle(void) {
  if (cursor_active_handle != NULL) {
    SDL_DestroyCursor(cursor_active_handle);
    cursor_active_handle = NULL;
  }
}

func SDL_Cursor* cursor_create_from_icon(icon id) {
  profile_func_begin;
  SDL_SystemCursor system_cursor = SDL_SYSTEM_CURSOR_DEFAULT;
  if (icon_internal_get_system_cursor(id, &system_cursor)) {
    SDL_Cursor* cursor_ptr = SDL_CreateSystemCursor(system_cursor);
    profile_func_end;
    return cursor_ptr;
  }

  SDL_Surface* surface = icon_internal_get_surface(id);
  if (surface == NULL) {
    profile_func_end;
    return NULL;
  }

  i32 hot_xpos = 0;
  i32 hot_ypos = 0;
  if (!icon_get_hotspot(id, &hot_xpos, &hot_ypos)) {
    profile_func_end;
    return NULL;
  }

  SDL_Cursor* cursor_ptr = SDL_CreateColorCursor(surface, hot_xpos, hot_ypos);
  profile_func_end;
  return cursor_ptr;
}

func b32 cursor_set_icon(icon id) {
  profile_func_begin;
  if (!icon_id_is_valid(id)) {
    thread_log_error("Rejected cursor icon set for invalid icon");
    profile_func_end;
    return false;
  }

  SDL_Cursor* cursor_ptr = cursor_create_from_icon(id);
  if (cursor_ptr == NULL) {
    thread_log_error("Failed to create cursor from icon error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  cursor_release_active_handle();
  SDL_SetCursor(cursor_ptr);
  cursor_active_handle = cursor_ptr;
  profile_func_end;
  return true;
}

func b32 cursor_reset_icon(void) {
  profile_func_begin;
  SDL_Cursor* cursor_ptr = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  if (cursor_ptr == NULL) {
    thread_log_error("Failed to reset cursor icon error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  cursor_release_active_handle();
  SDL_SetCursor(cursor_ptr);
  cursor_active_handle = cursor_ptr;
  profile_func_end;
  return true;
}

func b32 cursor_set_visible(b32 visible) {
  profile_func_begin;
  b32 result = visible != 0 ? (b32)SDL_ShowCursor() : (b32)SDL_HideCursor();
  profile_func_end;
  return result != 0 ? true : false;
}

func b32 cursor_is_visible(void) {
  return SDL_CursorVisible();
}

func b32 cursor_set_capture(window opt_window, b32 enabled) {
  profile_func_begin;
  SDL_Window* window_ptr = cursor_resolve_window(opt_window);
  if (window_ptr == NULL) {
    thread_log_warn("Rejected cursor capture request without target window");
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowMouseGrab(window_ptr, enabled != 0) ? true : false;
  if (!result) {
    thread_log_error("Failed to set cursor capture enabled=%u error=%s", (u32)enabled, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 cursor_is_captured(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = cursor_resolve_window(opt_window);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = (SDL_GetWindowFlags(window_ptr) & SDL_WINDOW_MOUSE_GRABBED) != 0 ? true : false;
  profile_func_end;
  return result;
}

func b32 cursor_set_relative_mode(window opt_window, b32 enabled) {
  profile_func_begin;
  SDL_Window* window_ptr = cursor_resolve_window(opt_window);
  if (window_ptr == NULL) {
    thread_log_warn("Rejected cursor relative mode request without target window");
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowRelativeMouseMode(window_ptr, enabled != 0) ? true : false;
  if (!result) {
    thread_log_error("Failed to set cursor relative mode enabled=%u error=%s", (u32)enabled, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 cursor_is_relative_mode(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = cursor_resolve_window(opt_window);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetWindowRelativeMouseMode(window_ptr) ? true : false;
  profile_func_end;
  return result;
}

func b32 cursor_warp(window opt_window, f32 xpos, f32 ypos) {
  profile_func_begin;
  if (window_id_is_valid(opt_window)) {
    SDL_Window* window_ptr = cursor_resolve_window(opt_window);
    if (window_ptr == NULL) {
      profile_func_end;
      return false;
    }

    SDL_WarpMouseInWindow(window_ptr, xpos, ypos);
    profile_func_end;
    return true;
  }

  b32 result = SDL_WarpMouseGlobal(xpos, ypos) ? true : false;
  if (!result) {
    thread_log_warn("Failed to warp cursor globally error=%s", SDL_GetError());
  }
  profile_func_end;
  return result;
}

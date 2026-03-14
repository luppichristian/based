// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/text_input.h"
#include "../internal.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 text_input_start(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StartTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 text_input_stop(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StopTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 text_input_is_active(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_TextInputActive(window_ptr) ? true : false;
  profile_func_end;
  return result;
}

func b32 text_input_set_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  SDL_Rect area = {.x = xpos, .y = ypos, .w = width, .h = height};
  b32 result = SDL_SetTextInputArea(window_ptr, &area, 0);
  profile_func_end;
  return result;
}

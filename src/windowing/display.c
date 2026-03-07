// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/display.h"
#include "../sdl3_include.h"

func b32 display_id_is_valid(display src) {
  return src != NULL;
}

func display display_from_native_id(up native_id) {
  return (display)(up)native_id;
}

func up display_to_native_id(display src) {
  return (up)src;
}

func sz display_get_count(void) {
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 display_get_id(sz idx, display* out_id) {
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = display_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  return found;
}

func display display_get_primary_id(void) {
  return display_from_native_id((up)SDL_GetPrimaryDisplay());
}

func cstr8 display_get_name(display id) {
  if (!display_id_is_valid(id)) {
    return NULL;
  }

  return SDL_GetDisplayName((SDL_DisplayID)display_to_native_id(id));
}

func b32 display_get_bounds(display id, display_rect* out_rect) {
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  return 1;
}

func b32 display_get_usable_bounds(display id, display_rect* out_rect) {
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayUsableBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  return 1;
}


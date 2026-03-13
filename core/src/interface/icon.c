// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/icon.h"
#include "context/thread_ctx.h"
#include "memory/memops.h"
#include "memory/vmem.h"
#include "basic/profiler.h"
#include "icon_internal.h"

typedef enum icon_kind {
  ICON_KIND_RGBA = 1,
  ICON_KIND_SYSTEM = 2,
} icon_kind;

typedef struct icon_state {
  icon_kind kind;
  i32 width;
  i32 height;
  i32 hot_xpos;
  i32 hot_ypos;
  icon_system system_icon;
  SDL_Surface* surface;
} icon_state;

func b32 icon_system_is_valid(icon_system system_icon) {
  return system_icon >= ICON_SYSTEM_DEFAULT && system_icon <= ICON_SYSTEM_W_RESIZE;
}

func SDL_SystemCursor icon_system_to_native(icon_system system_icon) {
  return (SDL_SystemCursor)system_icon;
}

func icon_state* icon_internal_resolve(icon id) {
  if (!icon_id_is_valid(id)) {
    return NULL;
  }

  return (icon_state*)id;
}

func SDL_Surface* icon_internal_get_surface(icon id) {
  icon_state* state = icon_internal_resolve(id);
  if (state == NULL || state->kind != ICON_KIND_RGBA) {
    return NULL;
  }

  return state->surface;
}

func b32 icon_internal_get_system_cursor(icon id, SDL_SystemCursor* out_system_cursor) {
  icon_state* state = icon_internal_resolve(id);
  if (out_system_cursor != NULL) {
    *out_system_cursor = SDL_SYSTEM_CURSOR_DEFAULT;
  }

  if (state == NULL || state->kind != ICON_KIND_SYSTEM || out_system_cursor == NULL) {
    return false;
  }

  *out_system_cursor = icon_system_to_native(state->system_icon);
  return true;
}

func b32 icon_id_is_valid(icon src) {
  return src != NULL;
}

func icon icon_create_rgba(i32 width, i32 height, const u8* rgba_pixels, i32 hot_xpos, i32 hot_ypos) {
  profile_func_begin;
  if (width <= 0 || height <= 0 || rgba_pixels == NULL || hot_xpos < 0 || hot_ypos < 0 || hot_xpos >= width || hot_ypos >= height) {
    thread_log_error("Rejected RGBA icon creation width=%d height=%d pixels=%p hotspot=(%d,%d)",
                     width,
                     height,
                     (const void*)rgba_pixels,
                     hot_xpos,
                     hot_ypos);
    profile_func_end;
    return NULL;
  }

  icon_state* state = (icon_state*)vmem_alloc(size_of(icon_state));
  if (state == NULL) {
    thread_log_error("Failed to allocate icon state width=%d height=%d", width, height);
    profile_func_end;
    return NULL;
  }

  mem_zero(state, size_of(icon_state));
  state->surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
  if (state->surface == NULL) {
    thread_log_error("Failed to create icon surface width=%d height=%d error=%s", width, height, SDL_GetError());
    vmem_free(state, size_of(icon_state));
    profile_func_end;
    return NULL;
  }

  if (!SDL_LockSurface(state->surface)) {
    thread_log_error("Failed to lock icon surface width=%d height=%d error=%s", width, height, SDL_GetError());
    SDL_DestroySurface(state->surface);
    vmem_free(state, size_of(icon_state));
    profile_func_end;
    return NULL;
  }

  safe_for (i32 row_idx = 0; row_idx < height; row_idx++) {
    u8* dst_row = (u8*)state->surface->pixels + ((sz)row_idx * (sz)state->surface->pitch);
    const u8* src_row = rgba_pixels + ((sz)row_idx * (sz)width * 4u);
    mem_cpy(dst_row, src_row, (sz)width * 4u);
  }
  SDL_UnlockSurface(state->surface);

  state->kind = ICON_KIND_RGBA;
  state->width = width;
  state->height = height;
  state->hot_xpos = hot_xpos;
  state->hot_ypos = hot_ypos;
  thread_log_info("Created RGBA icon width=%d height=%d hotspot=(%d,%d)", width, height, hot_xpos, hot_ypos);
  profile_func_end;
  return (icon)state;
}

func icon icon_create_system(icon_system system_icon) {
  profile_func_begin;
  if (!icon_system_is_valid(system_icon)) {
    thread_log_error("Rejected system icon creation kind=%u", (u32)system_icon);
    profile_func_end;
    return NULL;
  }

  icon_state* state = (icon_state*)vmem_alloc(size_of(icon_state));
  if (state == NULL) {
    thread_log_error("Failed to allocate system icon state kind=%u", (u32)system_icon);
    profile_func_end;
    return NULL;
  }

  mem_zero(state, size_of(icon_state));
  state->kind = ICON_KIND_SYSTEM;
  state->system_icon = system_icon;
  thread_log_info("Created system icon kind=%u", (u32)system_icon);
  profile_func_end;
  return (icon)state;
}

func b32 icon_destroy(icon id) {
  profile_func_begin;
  icon_state* state = icon_internal_resolve(id);
  if (state == NULL) {
    thread_log_error("Rejected icon destroy for invalid icon");
    profile_func_end;
    return false;
  }

  if (state->surface != NULL) {
    SDL_DestroySurface(state->surface);
    state->surface = NULL;
  }

  vmem_free(state, size_of(icon_state));
  profile_func_end;
  return true;
}

func b32 icon_get_size(icon id, i32* out_width, i32* out_height) {
  profile_func_begin;
  icon_state* state = icon_internal_resolve(id);
  if (out_width != NULL) {
    *out_width = 0;
  }
  if (out_height != NULL) {
    *out_height = 0;
  }

  if (state == NULL || out_width == NULL || out_height == NULL) {
    profile_func_end;
    return false;
  }

  *out_width = state->width;
  *out_height = state->height;
  profile_func_end;
  return true;
}

func b32 icon_get_hotspot(icon id, i32* out_xpos, i32* out_ypos) {
  profile_func_begin;
  icon_state* state = icon_internal_resolve(id);
  if (out_xpos != NULL) {
    *out_xpos = 0;
  }
  if (out_ypos != NULL) {
    *out_ypos = 0;
  }

  if (state == NULL || out_xpos == NULL || out_ypos == NULL) {
    profile_func_end;
    return false;
  }

  *out_xpos = state->hot_xpos;
  *out_ypos = state->hot_ypos;
  profile_func_end;
  return true;
}

func b32 icon_get_system(icon id, icon_system* out_system_icon) {
  profile_func_begin;
  icon_state* state = icon_internal_resolve(id);
  if (out_system_icon != NULL) {
    *out_system_icon = ICON_SYSTEM_DEFAULT;
  }

  if (state == NULL || out_system_icon == NULL || state->kind != ICON_KIND_SYSTEM) {
    profile_func_end;
    return false;
  }

  *out_system_icon = state->system_icon;
  profile_func_end;
  return true;
}

func b32 icon_is_rgba(icon id) {
  icon_state* state = icon_internal_resolve(id);
  return state != NULL && state->kind == ICON_KIND_RGBA ? true : false;
}

func b32 icon_is_system(icon id) {
  icon_state* state = icon_internal_resolve(id);
  return state != NULL && state->kind == ICON_KIND_SYSTEM ? true : false;
}

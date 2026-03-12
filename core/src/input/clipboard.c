// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/clipboard.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "strings/cstrings.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func void clipboard_clear_output(c8* out_text, sz out_capacity) {
  profile_func_begin;
  if (!out_text || !out_capacity) {
    profile_func_end;
    return;
  }

  out_text[0] = '\0';
  profile_func_end;
}

func b32 clipboard_has_text(void) {
  return SDL_HasClipboardText() ? true : false;
}

func b32 clipboard_set_text(cstr8 src) {
  profile_func_begin;
  if (!src) {
    thread_log_error("Rejected clipboard text set for NULL input");
    profile_func_end;
    return false;
  }
  assert(src != NULL);

  b32 success = SDL_SetClipboardText(src) ? true : false;
  if (!success) {
    thread_log_warn("Failed to set clipboard text error=%s", SDL_GetError());
  } else {
    thread_log_trace("Set clipboard text size=%zu", (size_t)cstr8_len(src));
  }
  profile_func_end;
  return success;
}

func b32 clipboard_get_text(c8* out_text, sz out_capacity) {
  profile_func_begin;
  c8* source_text = NULL;
  b32 result = false;

  if (!out_text || !out_capacity) {
    thread_log_error("Rejected clipboard text read with invalid output buffer");
    profile_func_end;
    return false;
  }
  assert(out_text != NULL);

  clipboard_clear_output(out_text, out_capacity);

  source_text = SDL_GetClipboardText();
  if (!source_text) {
    thread_log_warn("Failed to get clipboard text error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  cstr8_cpy(out_text, out_capacity, source_text);
  result = true;
  SDL_free(source_text);
  thread_log_trace("Read clipboard text size=%zu", (size_t)cstr8_len(out_text));
  profile_func_end;
  return result;
}

func b32 clipboard_clear(void) {
  b32 success = SDL_ClearClipboardData() ? true : false;
  if (!success) {
    thread_log_warn("Failed to clear clipboard data error=%s", SDL_GetError());
  } else {
    thread_log_trace("Cleared clipboard data");
  }
  return success;
}

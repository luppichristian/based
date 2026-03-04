// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/clipboard.h"
#include "strings/cstrings.h"
#include "../sdl3_include.h"

func void clipboard_clear_output(c8* out_text, sz out_capacity) {
  if (!out_text || !out_capacity) {
    return;
  }

  out_text[0] = '\0';
}

func b32 clipboard_has_text(void) {
  return SDL_HasClipboardText() ? 1 : 0;
}

func b32 clipboard_set_text(const c8* src) {
  if (!src) {
    return 0;
  }

  return SDL_SetClipboardText(src) ? 1 : 0;
}

func b32 clipboard_get_text(c8* out_text, sz out_capacity) {
  c8* source_text = NULL;
  b32 result = 0;

  if (!out_text || !out_capacity) {
    return 0;
  }

  clipboard_clear_output(out_text, out_capacity);

  source_text = SDL_GetClipboardText();
  if (!source_text) {
    return 0;
  }

  cstr8_copy(out_text, out_capacity, source_text);
  result = 1;
  SDL_free(source_text);
  return result;
}

func b32 clipboard_clear(void) {
  return SDL_ClearClipboardData() ? 1 : 0;
}

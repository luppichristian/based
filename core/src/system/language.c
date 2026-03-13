// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/language.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "context/thread_ctx.h"
#include "input/devices.h"
#include "memory/memops.h"
#include "platform_includes.h"
#include "../sdl3_include.h"

func void language_copy_tag(c8* dst, sz dst_cap, cstr8 language_name, cstr8 country_name) {
  profile_func_begin;
  if (dst == NULL || dst_cap == 0) {
    profile_func_end;
    return;
  }

  cstr8_clear(dst);
  if (language_name == NULL || language_name[0] == '\0') {
    profile_func_end;
    return;
  }

  cstr8_cpy(dst, dst_cap, language_name);
  cstr8_replace_char(dst, '_', '-');
  if (country_name != NULL && country_name[0] != '\0') {
    if (!cstr8_ends_with(dst, country_name)) {
      cstr8_cat(dst, dst_cap, "-");
      cstr8_cat(dst, dst_cap, country_name);
    }
  }
  profile_func_end;
}

func void language_query_sdl_desktop(language_info* out_info) {
  profile_func_begin;
  int locale_count = 0;
  SDL_Locale** locales = SDL_GetPreferredLocales(&locale_count);
  if (locales == NULL || locale_count <= 0 || locales[0] == NULL || locales[0]->language == NULL) {
    thread_log_debug("SDL preferred locales unavailable");
    if (locales != NULL) {
      SDL_free(locales);
    }
    profile_func_end;
    return;
  }

  language_copy_tag(out_info->desktop_language,
                    size_of(out_info->desktop_language),
                    locales[0]->language,
                    locales[0]->country);
  SDL_free(locales);
  profile_func_end;
}

func void language_query_keyboard_fallback(language_info* out_info) {
  profile_func_begin;
  if (out_info->keyboard_language[0] == '\0' && out_info->desktop_language[0] != '\0') {
    cstr8_cpy(out_info->keyboard_language,
              size_of(out_info->keyboard_language),
              out_info->desktop_language);
  }

  if (out_info->keyboard_layout_name[0] == '\0') {
    if (devices_get_count(DEVICE_TYPE_KEYBOARD) > 0) {
      device_info keyboard_info = {0};
      device keyboard_id = devices_get_device(DEVICE_TYPE_KEYBOARD, 0);
      if (device_is_valid(keyboard_id) && devices_get_info(keyboard_id, &keyboard_info)) {
        cstr8_cpy(out_info->keyboard_layout_name,
                  size_of(out_info->keyboard_layout_name),
                  keyboard_info.name);
      }
    }
  }

  if (out_info->keyboard_layout_name[0] == '\0' && out_info->keyboard_language[0] != '\0') {
    cstr8_cpy(out_info->keyboard_layout_name,
              size_of(out_info->keyboard_layout_name),
              out_info->keyboard_language);
  }
  profile_func_end;
}

#if defined(PLATFORM_WINDOWS)
func void language_copy_windows_wide(c8* dst, sz dst_cap, const WCHAR* src) {
  profile_func_begin;
  if (dst == NULL || dst_cap == 0) {
    profile_func_end;
    return;
  }

  cstr8_clear(dst);
  if (src == NULL || src[0] == 0) {
    profile_func_end;
    return;
  }

  cstr16_to_cstr8((const c16*)src, dst, dst_cap);
  cstr8_replace_char(dst, '_', '-');
  profile_func_end;
}

func void language_query_windows(language_info* out_info) {
  profile_func_begin;
  WCHAR wide_buffer[LOCALE_NAME_MAX_LENGTH] = {0};

  if (GetUserDefaultLocaleName(wide_buffer, LOCALE_NAME_MAX_LENGTH) > 0) {
    language_copy_windows_wide(
        out_info->desktop_language, size_of(out_info->desktop_language), wide_buffer);
  } else {
    thread_log_debug("Failed to query Windows desktop language");
  }

  HKL keyboard_layout = GetKeyboardLayout(0);
  LANGID language_id = LOWORD((DWORD_PTR)keyboard_layout);
  LCID locale_id = MAKELCID(language_id, SORT_DEFAULT);
  if (LCIDToLocaleName(locale_id, wide_buffer, LOCALE_NAME_MAX_LENGTH, 0) > 0) {
    language_copy_windows_wide(
        out_info->keyboard_language, size_of(out_info->keyboard_language), wide_buffer);
  } else {
    thread_log_debug("Failed to query Windows keyboard language");
  }

  WCHAR layout_name[KL_NAMELENGTH] = {0};
  if (GetKeyboardLayoutNameW(layout_name) != 0) {
    language_copy_windows_wide(
        out_info->keyboard_layout_name, size_of(out_info->keyboard_layout_name), layout_name);
  } else {
    thread_log_debug("Failed to query Windows keyboard layout identifier");
  }
  profile_func_end;
}
#endif

func b32 language_query(language_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_info != NULL);

  mem_zero(out_info, size_of(*out_info));

#if defined(PLATFORM_WINDOWS)
  language_query_windows(out_info);
#else
  language_query_sdl_desktop(out_info);
#endif

  if (out_info->desktop_language[0] == '\0') {
    language_query_sdl_desktop(out_info);
  }

  language_query_keyboard_fallback(out_info);

  if (out_info->desktop_language[0] == '\0') {
    cstr8_cpy(out_info->desktop_language, size_of(out_info->desktop_language), "unknown");
  }
  if (out_info->keyboard_language[0] == '\0') {
    cstr8_cpy(out_info->keyboard_language, size_of(out_info->keyboard_language), out_info->desktop_language);
  }
  if (out_info->keyboard_layout_name[0] == '\0') {
    cstr8_cpy(out_info->keyboard_layout_name,
              size_of(out_info->keyboard_layout_name),
              out_info->keyboard_language);
  }

  thread_log_trace("Queried language info desktop=%s keyboard=%s",
                   out_info->desktop_language,
                   out_info->keyboard_language);
  profile_func_end;
  return true;
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/path.h"

#include "../sdl3_include.h"

#include "strings/char.h"

#include <string.h>

func c8* path_mut_buf(path* value) {
  return value->buf;
}

func const c8* path_buf(const path* value) {
  return value->buf;
}

func path path_empty_value(void) {
  path value;
  cstr8_clear(value.buf);
  return value;
}

func b32 path_is_separator(c8 chr) {
  return chr == '/' || chr == '\\';
}

func sz path_root_length_cstr(const c8* src) {
  if (src[0] == '\0') {
    return 0;
  }

  if (path_is_separator(src[0])) {
    if (path_is_separator(src[1])) {
      return 2;
    }
    return 1;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    return 3;
  }

  return 0;
}

func sz path_trimmed_length_cstr(const c8* src) {
  sz root_len = path_root_length_cstr(src);
  sz src_len = cstr8_len(src);

  while (src_len > root_len && path_is_separator(src[src_len - 1])) {
    src_len -= 1;
  }

  return src_len;
}

func sz path_name_start_cstr(const c8* src) {
  sz root_len = path_root_length_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  if (src_len == 0) {
    return 0;
  }

  while (src_len > root_len) {
    if (path_is_separator(src[src_len - 1])) {
      break;
    }
    src_len -= 1;
  }

  return src_len;
}

func sz path_extension_start_cstr(const c8* src) {
  sz name_idx = path_name_start_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  while (src_len > name_idx) {
    if (src[src_len - 1] == '.') {
      if (src_len - 1 == name_idx) {
        return SZ_MAX;
      }
      return src_len - 1;
    }

    if (path_is_separator(src[src_len - 1])) {
      break;
    }

    src_len -= 1;
  }

  return SZ_MAX;
}

func b32 path_is_absolute_cstr(const c8* src) {
  if (src[0] == '\0') {
    return 0;
  }

  if (path_is_separator(src[0])) {
    return 1;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    return 1;
  }

  return 0;
}

func path path_from_cstr(const c8* src) {
  path value;
  str8_from_cstr(value.buf, PATH_CAP, src);
  return value;
}

func path path_from_str8(str8 src) {
  path value = path_empty_value();
  cstr8_copy_n(value.buf, PATH_CAP, src.ptr, src.size);
  return value;
}

func path path_join_cstr(const path* lhs, const c8* rhs) {
  path result;

  if (path_is_absolute_cstr(rhs) || cstr8_is_empty(path_buf(lhs))) {
    cstr8_copy(result.buf, PATH_CAP, rhs);
    path_normalize(&result);
    return result;
  }

  cstr8_copy(result.buf, PATH_CAP, path_buf(lhs));
  path_remove_trailing_slash(&result);

  while (path_is_separator(*rhs)) {
    rhs += 1;
  }

  sz dst_len = cstr8_len(result.buf);
  if (*rhs != '\0' && dst_len > 0 && !path_is_separator(result.buf[dst_len - 1]) && dst_len + 1 < PATH_CAP) {
    cstr8_append_char(result.buf, PATH_CAP, '/');
  }

  cstr8_concat(result.buf, PATH_CAP, rhs);
  path_normalize(&result);
  return result;
}

func path path_join(const path* lhs, const path* rhs) {
  return path_join_cstr(lhs, path_buf(rhs));
}

func sz path_append_cstr(path* dst, const c8* src) {
  *dst = path_join_cstr(dst, src);
  return cstr8_len(path_mut_buf(dst));
}

func sz path_append(path* dst, const path* src) {
  *dst = path_join(dst, src);
  return cstr8_len(path_mut_buf(dst));
}

func void path_normalize(path* src) {
  sz root_len = path_root_length_cstr(path_buf(src));
  sz read_idx = 0;
  sz write_idx = 0;
  b32 prev_sep = 0;

  while (src->buf[read_idx] != '\0') {
    c8 chr = src->buf[read_idx];
    if (chr == '\\') {
      chr = '/';
    }

    if (chr == '/') {
      if (prev_sep && !(write_idx < root_len && root_len == 2)) {
        read_idx += 1;
        continue;
      }
      prev_sep = 1;
    } else {
      prev_sep = 0;
    }

    src->buf[write_idx] = chr;
    write_idx += 1;
    read_idx += 1;
  }

  src->buf[write_idx] = '\0';
}

func b32 path_ends_with(const path* src, const c8* suffix) {
  return cstr8_ends_with(path_buf(src), suffix);
}

func b32 path_is_absolute(const path* src) {
  return path_is_absolute_cstr(path_buf(src));
}

func void path_remove_extension(path* src) {
  sz dot_idx = path_extension_start_cstr(path_buf(src));
  if (dot_idx != SZ_MAX) {
    cstr8_truncate(path_mut_buf(src), dot_idx);
  }
}

func void path_remove_name(path* src) {
  sz cut_idx = path_name_start_cstr(path_buf(src));
  cstr8_truncate(path_mut_buf(src), cut_idx);
  path_remove_trailing_slash(src);
}

func void path_remove_directory(path* src) {
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  if (name_idx == 0) {
    src->buf[src_len] = '\0';
    return;
  }

  memmove(src->buf, src->buf + name_idx, src_len - name_idx);
  src->buf[src_len - name_idx] = '\0';
}

func void path_remove_trailing_slash(path* src) {
  sz root_len = path_root_length_cstr(path_buf(src));
  sz src_len = cstr8_len(path_buf(src));

  while (src_len > root_len && path_is_separator(src->buf[src_len - 1])) {
    src_len -= 1;
  }

  src->buf[src_len] = '\0';
}

func path path_get_extension(const path* src) {
  path result = path_empty_value();
  sz dot_idx = path_extension_start_cstr(path_buf(src));

  if (dot_idx != SZ_MAX) {
    cstr8_copy(result.buf, PATH_CAP, path_buf(src) + dot_idx);
  }

  return result;
}

func path path_get_name(const path* src) {
  path result = path_empty_value();
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  cstr8_copy_n(result.buf, PATH_CAP, path_buf(src) + name_idx, src_len - name_idx);
  return result;
}

func path path_get_basename(const path* src) {
  path result = path_get_name(src);
  path_remove_extension(&result);
  return result;
}

func path path_get_directory(const path* src) {
  path result = *src;
  path_remove_name(&result);
  return result;
}

func path path_get_common(const path* src_list, sz path_count) {
  path result = path_empty_value();
  path current;
  sz item_idx = 0;

  if (path_count == 0) {
    return result;
  }

  result = src_list[0];
  path_normalize(&result);

  for (item_idx = 1; item_idx < path_count; item_idx += 1) {
    current = src_list[item_idx];
    path_normalize(&current);
    cstr8_common_prefix(result.buf, current.buf, result.buf, PATH_CAP);
  }

  path_remove_name(&result);
  return result;
}

func b32 path_exists(const path* src) {
  SDL_PathInfo path_info;
  return SDL_GetPathInfo(path_buf(src), &path_info) ? 1 : 0;
}

func b32 path_remove(const path* src) {
  return SDL_RemovePath(path_buf(src)) ? 1 : 0;
}

func b32 path_rename(const path* old_src, const path* new_src) {
  return SDL_RenamePath(path_buf(old_src), path_buf(new_src)) ? 1 : 0;
}

func timestamp path_get_last_write_time(const path* src) {
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(path_buf(src), &path_info)) {
    return timestamp_zero();
  }

  return timestamp_from_microseconds((i64)SDL_NS_TO_US(path_info.modify_time));
}

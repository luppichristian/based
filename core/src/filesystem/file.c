// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/file.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"

#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <stdio.h>

#if defined(PLATFORM_WINDOWS)
#  include <windows.h>
#endif

func b32 file_paths_equal(const path* lhs, const path* rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return cstr8_cmp(lhs->buf, rhs->buf) == 0 ? 1 : 0;
}

func b32 file_replace_path(const path* src, const path* dst) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS)
  profile_func_end;
  return MoveFileExA(src->buf, dst->buf, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) ? 1 : 0;
#elif defined(PLATFORM_UNIX)
  profile_func_end;
  return rename(src->buf, dst->buf) == 0 ? 1 : 0;
#else
  if (path_exists(dst) && !path_remove(dst)) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return SDL_RenamePath(src->buf, dst->buf) ? 1 : 0;
#endif
}

func b32 file_make_temp_path(const path* src, path* out_path) {
  profile_func_begin;
  path tmp_path;
  u32 attempt_idx = 0;

  if (src == NULL || out_path == NULL) {
    profile_func_end;
    return 0;
  }

  for (attempt_idx = 0; attempt_idx < 32; attempt_idx += 1) {
    tmp_path = *src;
    if (!cstr8_append_format(tmp_path.buf, size_of(tmp_path.buf), ".tmp.%u", attempt_idx)) {
      profile_func_end;
      return 0;
    }

    if (!path_exists(&tmp_path)) {
      *out_path = tmp_path;
      profile_func_end;
      return 1;
    }
  }

  profile_func_end;
  return 0;
}

func cstr8 file_path_cstr(const path* src) {
  profile_func_begin;
  if (src == NULL) {
    profile_func_end;
    return "";
  }
  profile_func_end;
  return src->buf;
}

func b32 file_create(const path* src) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return 0;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  if (file_ptr == NULL) {
    thread_log_error("file_create: failed path=%s", src->buf);
    profile_func_end;
    return 0;
  }

  SDL_CloseIO(file_ptr);
  thread_log_trace("file_create: path=%s", src->buf);
  profile_func_end;
  return 1;
}

func b32 file_delete(const path* src) {
  profile_func_begin;
  if (!file_exists(src)) {
    profile_func_end;
    return 0;
  }
  assert(src != NULL);

  profile_func_end;
  return SDL_RemovePath(file_path_cstr(src)) ? 1 : 0;
}

func b32 file_rename(const path* old_src, const path* new_src) {
  profile_func_begin;
  if (!file_exists(old_src)) {
    profile_func_end;
    return 0;
  }
  if (new_src == NULL || new_src->buf[0] == '\0') {
    profile_func_end;
    return 0;
  }
  assert(new_src->buf[0] != '\0');

  profile_func_end;
  return SDL_RenamePath(file_path_cstr(old_src), file_path_cstr(new_src)) ? 1 : 0;
}

func b32 file_copy(const path* src, const path* dst, b32 overwrite_existing) {
  profile_func_begin;
  SDL_IOStream* src_file = NULL;
  SDL_IOStream* dst_file = NULL;
  u8 copy_buf[16 * 1024];
  sz read_size = 0;

  if (!file_exists(src) || dst == NULL) {
    profile_func_end;
    return 0;
  }
  assert(src != NULL);
  assert(dst != NULL);

  if (file_paths_equal(src, dst)) {
    profile_func_end;
    return 1;
  }

  if (path_exists(dst)) {
    if (!overwrite_existing || !file_exists(dst)) {
      profile_func_end;
      return 0;
    }
  }

  src_file = SDL_IOFromFile(file_path_cstr(src), "rb");
  if (src_file == NULL) {
    profile_func_end;
    return 0;
  }

  dst_file = SDL_IOFromFile(file_path_cstr(dst), "wb");
  if (dst_file == NULL) {
    SDL_CloseIO(src_file);
    profile_func_end;
    return 0;
  }

  for (;;) {
    read_size = (sz)SDL_ReadIO(src_file, copy_buf, size_of(copy_buf));
    if (read_size == 0) {
      break;
    }

    if ((sz)SDL_WriteIO(dst_file, copy_buf, (size_t)read_size) != read_size) {
      SDL_CloseIO(dst_file);
      SDL_CloseIO(src_file);
      profile_func_end;
      return 0;
    }
  }

  if (!SDL_CloseIO(dst_file)) {
    SDL_CloseIO(src_file);
    profile_func_end;
    return 0;
  }

  SDL_CloseIO(src_file);
  profile_func_end;
  return 1;
}

func b32 file_exists(const path* src) {
  profile_func_begin;
  SDL_PathInfo path_info;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return 0;
  }

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info)) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return path_info.type == SDL_PATHTYPE_FILE ? 1 : 0;
}

func b32 file_get_size(const path* src, sz* out_size) {
  profile_func_begin;
  SDL_PathInfo path_info;

  if (out_size == NULL || !file_exists(src)) {
    profile_func_end;
    return 0;
  }
  assert(out_size != NULL);

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info) || path_info.type != SDL_PATHTYPE_FILE) {
    profile_func_end;
    return 0;
  }

  *out_size = (sz)path_info.size;
  profile_func_end;
  return 1;
}

func b32 file_read_all(const path* src, allocator* alloc, buffer* out_data) {
  profile_func_begin;
  SDL_IOStream* file_ptr = NULL;
  void* data_ptr = NULL;
  sz file_size = 0;
  sz read_size = 0;

  if (alloc == NULL || out_data == NULL) {
    profile_func_end;
    return 0;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    profile_func_end;
    return 0;
  }
  assert(alloc->alloc_fn != NULL);
  assert(alloc->dealloc_fn != NULL);

  out_data->ptr = NULL;
  out_data->size = 0;

  if (!file_get_size(src, &file_size)) {
    profile_func_end;
    return 0;
  }

  file_ptr = SDL_IOFromFile(file_path_cstr(src), "rb");
  if (file_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  if (file_size > 0) {
    data_ptr = allocator_alloc(*alloc, file_size);
    if (data_ptr == NULL) {
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return 0;
    }

    read_size = (sz)SDL_ReadIO(file_ptr, data_ptr, (size_t)file_size);
    if (read_size != file_size) {
      allocator_dealloc(*alloc, data_ptr, file_size);
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return 0;
    }
  }

  SDL_CloseIO(file_ptr);
  out_data->ptr = data_ptr;
  out_data->size = file_size;
  thread_log_trace("file_read_all: path=%s size=%zu", src != NULL ? src->buf : "<null>", (size_t)file_size);
  profile_func_end;
  return 1;
}

func b32 file_write_all(const path* src, buffer data) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0' || (data.size > 0 && data.ptr == NULL)) {
    profile_func_end;
    return 0;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  sz write_size = 0;

  if (file_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return 0;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    profile_func_end;
    return 0;
  }
  thread_log_trace("file_write_all: path=%s size=%zu", src->buf, (size_t)data.size);
  profile_func_end;
  return 1;
}

func b32 file_append_all(const path* src, buffer data) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0' || (data.size > 0 && data.ptr == NULL)) {
    profile_func_end;
    return 0;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "ab");
  sz write_size = 0;

  if (file_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return 0;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    profile_func_end;
    return 0;
  }
  profile_func_end;
  return 1;
}

func b32 file_write_all_atomic(const path* src, buffer data) {
  profile_func_begin;
  path tmp_path;

  if (src == NULL) {
    profile_func_end;
    return 0;
  }

  if (!file_make_temp_path(src, &tmp_path)) {
    profile_func_end;
    return 0;
  }

  if (!file_write_all(&tmp_path, data)) {
    (void)path_remove(&tmp_path);
    profile_func_end;
    return 0;
  }

  if (!file_replace_path(&tmp_path, src)) {
    (void)path_remove(&tmp_path);
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return 1;
}

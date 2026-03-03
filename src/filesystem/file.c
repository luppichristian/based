// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/file.h"

#include "../sdl3_include.h"

func const c8* file_path_cstr(const path* src) {
  if (src == NULL) {
    return "";
  }
  return src->buf;
}

func b32 file_create(const path* src) {
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  if (file_ptr == NULL) {
    return 0;
  }

  SDL_CloseIO(file_ptr);
  return 1;
}

func b32 file_delete(const path* src) {
  if (!file_exists(src)) {
    return 0;
  }

  return SDL_RemovePath(file_path_cstr(src)) ? 1 : 0;
}

func b32 file_rename(const path* old_src, const path* new_src) {
  if (!file_exists(old_src)) {
    return 0;
  }

  return SDL_RenamePath(file_path_cstr(old_src), file_path_cstr(new_src)) ? 1 : 0;
}

func b32 file_exists(const path* src) {
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info)) {
    return 0;
  }

  return path_info.type == SDL_PATHTYPE_FILE ? 1 : 0;
}

func b32 file_get_size(const path* src, sz* out_size) {
  SDL_PathInfo path_info;

  if (out_size == NULL || !file_exists(src)) {
    return 0;
  }

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info) || path_info.type != SDL_PATHTYPE_FILE) {
    return 0;
  }

  *out_size = (sz)path_info.size;
  return 1;
}

func b32 file_read_all(const path* src, allocator* alloc, buffer* out_data) {
  SDL_IOStream* file_ptr = NULL;
  void* data_ptr = NULL;
  sz file_size = 0;
  sz read_size = 0;

  if (alloc == NULL || out_data == NULL) {
    return 0;
  }

  out_data->ptr = NULL;
  out_data->size = 0;

  if (!file_get_size(src, &file_size)) {
    return 0;
  }

  file_ptr = SDL_IOFromFile(file_path_cstr(src), "rb");
  if (file_ptr == NULL) {
    return 0;
  }

  if (file_size > 0) {
    data_ptr = allocator_alloc(alloc, file_size);
    if (data_ptr == NULL) {
      SDL_CloseIO(file_ptr);
      return 0;
    }

    read_size = (sz)SDL_ReadIO(file_ptr, data_ptr, (size_t)file_size);
    if (read_size != file_size) {
      allocator_dealloc(alloc, data_ptr, file_size);
      SDL_CloseIO(file_ptr);
      return 0;
    }
  }

  SDL_CloseIO(file_ptr);
  out_data->ptr = data_ptr;
  out_data->size = file_size;
  return 1;
}

func b32 file_write_all(const path* src, buffer data) {
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  sz write_size = 0;

  if (file_ptr == NULL) {
    return 0;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      SDL_CloseIO(file_ptr);
      return 0;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    return 0;
  }
  return 1;
}

func b32 file_append_all(const path* src, buffer data) {
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "ab");
  sz write_size = 0;

  if (file_ptr == NULL) {
    return 0;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      SDL_CloseIO(file_ptr);
      return 0;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    return 0;
  }
  return 1;
}

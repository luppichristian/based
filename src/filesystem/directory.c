// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/directory.h"

#include "filesystem/file.h"

#include "../sdl3_include.h"

#include <string.h>

typedef struct directory_iterate_state {
  path root_path;
  b32 recursive;
  directory_iterate_callback* callback;
  void* user_data;
  b32 stop_requested;
  b32 success;
} directory_iterate_state;

typedef struct directory_remove_state {
  b32 success;
} directory_remove_state;

typedef struct directory_copy_state {
  path dst_root;
  b32 overwrite_existing;
  b32 success;
} directory_copy_state;

func const c8* directory_path_cstr(const path* src) {
  if (src == NULL) {
    return "";
  }
  return src->buf;
}

func sz directory_root_length(const c8* src) {
  if (src == NULL || src[0] == '\0') {
    return 0;
  }

  if (src[0] == '/' || src[0] == '\\') {
    if (src[1] == '/' || src[1] == '\\') {
      return 2;
    }
    return 1;
  }

  if (((src[0] >= 'A' && src[0] <= 'Z') || (src[0] >= 'a' && src[0] <= 'z')) &&
      src[1] == ':' && (src[2] == '/' || src[2] == '\\')) {
    return 3;
  }

  return 0;
}

func path directory_relative_path(const path* root_path, const path* full_path) {
  path relative_path = *full_path;
  path root_copy = *root_path;
  sz root_len = 0;

  path_normalize(&relative_path);
  path_normalize(&root_copy);
  path_remove_trailing_slash(&root_copy);
  root_len = cstr8_len(root_copy.buf);

  if (root_len == 0) {
    return relative_path;
  }

  if (cstr8_cmp_n(relative_path.buf, root_copy.buf, root_len) == 0) {
    if (relative_path.buf[root_len] == '/') {
      memmove(
          relative_path.buf,
          relative_path.buf + root_len + 1,
          cstr8_len(relative_path.buf + root_len + 1) + 1);
      return relative_path;
    }

    if (relative_path.buf[root_len] == '\0') {
      cstr8_clear(relative_path.buf);
      return relative_path;
    }
  }

  return relative_path;
}

func b32 directory_emit_entry(
    directory_iterate_state* state,
    const path* full_path,
    b32 is_directory) {
  directory_entry entry;

  if (state->callback == NULL) {
    return 1;
  }

  entry.full_path = *full_path;
  entry.relative_path = directory_relative_path(&state->root_path, full_path);
  entry.is_directory = is_directory;
  return state->callback(&entry, state->user_data);
}

func SDL_EnumerationResult SDLCALL directory_enumerate_callback(
    void* user_data,
    const char* dirname,
    const char* fname) {
  directory_iterate_state* state = (directory_iterate_state*)user_data;
  SDL_PathInfo path_info;
  path dir_path;
  path name_path;
  path full_path;

  if (state == NULL || fname == NULL) {
    return SDL_ENUM_FAILURE;
  }

  if (state->stop_requested) {
    return SDL_ENUM_SUCCESS;
  }

  dir_path = path_from_cstr(dirname != NULL ? dirname : "");
  name_path = path_from_cstr(fname);
  full_path = path_join(&dir_path, &name_path);

  if (!SDL_GetPathInfo(full_path.buf, &path_info)) {
    state->success = 0;
    return SDL_ENUM_FAILURE;
  }

  if (!directory_emit_entry(state, &full_path, path_info.type == SDL_PATHTYPE_DIRECTORY ? 1 : 0)) {
    state->stop_requested = 1;
    return SDL_ENUM_SUCCESS;
  }

  if (state->recursive && path_info.type == SDL_PATHTYPE_DIRECTORY) {
    if (!SDL_EnumerateDirectory(full_path.buf, directory_enumerate_callback, state)) {
      if (state->stop_requested) {
        return SDL_ENUM_SUCCESS;
      }

      state->success = 0;
      return SDL_ENUM_FAILURE;
    }

    if (state->stop_requested) {
      return SDL_ENUM_SUCCESS;
    }
  }

  return SDL_ENUM_CONTINUE;
}

func SDL_EnumerationResult SDLCALL directory_remove_callback(
    void* user_data,
    const char* dirname,
    const char* fname) {
  directory_remove_state* state = (directory_remove_state*)user_data;
  SDL_PathInfo path_info;
  path dir_path;
  path name_path;
  path full_path;

  if (state == NULL || fname == NULL) {
    return SDL_ENUM_FAILURE;
  }

  dir_path = path_from_cstr(dirname != NULL ? dirname : "");
  name_path = path_from_cstr(fname);
  full_path = path_join(&dir_path, &name_path);

  if (!SDL_GetPathInfo(full_path.buf, &path_info)) {
    state->success = 0;
    return SDL_ENUM_FAILURE;
  }

  if (path_info.type == SDL_PATHTYPE_DIRECTORY) {
    if (!SDL_EnumerateDirectory(full_path.buf, directory_remove_callback, state)) {
      state->success = 0;
      return SDL_ENUM_FAILURE;
    }
  }

  if (!SDL_RemovePath(full_path.buf)) {
    state->success = 0;
    return SDL_ENUM_FAILURE;
  }

  return SDL_ENUM_CONTINUE;
}

func b32 directory_copy_entry(const directory_entry* entry, void* user_data) {
  directory_copy_state* state = (directory_copy_state*)user_data;
  path dst_path;

  if (state == NULL || entry == NULL) {
    return 0;
  }

  dst_path = path_join(&state->dst_root, &entry->relative_path);
  if (entry->is_directory) {
    state->success = directory_copy_recursive(
        &entry->full_path,
        &dst_path,
        state->overwrite_existing);
  } else {
    state->success = file_copy(&entry->full_path, &dst_path, state->overwrite_existing);
  }

  return state->success;
}

func b32 directory_path_is_same_or_child(const path* root_path, const path* child_path) {
  path root_abs = path_resolve(root_path);
  path child_abs = path_resolve(child_path);
  sz root_len = cstr8_len(root_abs.buf);

  if (root_len == 0) {
    return 0;
  }

  if (cstr8_cmp_n(root_abs.buf, child_abs.buf, root_len) != 0) {
    return 0;
  }

  return child_abs.buf[root_len] == '\0' || child_abs.buf[root_len] == '/' ? 1 : 0;
}

func b32 directory_create(const path* src) {
  if (SDL_CreateDirectory(directory_path_cstr(src))) {
    return 1;
  }

  return directory_exists(src);
}

func b32 directory_remove(const path* src) {
  if (!directory_exists(src)) {
    return 0;
  }

  return SDL_RemovePath(directory_path_cstr(src)) ? 1 : 0;
}

func b32 directory_rename(const path* old_src, const path* new_src) {
  if (!directory_exists(old_src)) {
    return 0;
  }

  return SDL_RenamePath(directory_path_cstr(old_src), directory_path_cstr(new_src)) ? 1 : 0;
}

func b32 directory_copy_recursive(const path* src, const path* dst, b32 overwrite_existing) {
  directory_copy_state state;
  path src_abs;
  path dst_abs;

  if (!directory_exists(src) || dst == NULL) {
    return 0;
  }

  src_abs = path_resolve(src);
  dst_abs = path_resolve(dst);
  if (directory_path_is_same_or_child(&src_abs, &dst_abs)) {
    return cstr8_cmp(src_abs.buf, dst_abs.buf) == 0 ? 1 : 0;
  }

  if (path_exists(dst)) {
    if (!directory_exists(dst)) {
      return 0;
    }
  } else if (!directory_create_recursive(dst)) {
    return 0;
  }

  memset(&state, 0, size_of(state));
  state.dst_root = *dst;
  state.overwrite_existing = overwrite_existing;
  state.success = 1;

  if (!directory_iterate(src, directory_copy_entry, &state)) {
    return 0;
  }

  return state.success;
}

func b32 directory_exists(const path* src) {
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(directory_path_cstr(src), &path_info)) {
    return 0;
  }

  return path_info.type == SDL_PATHTYPE_DIRECTORY ? 1 : 0;
}

func b32 directory_create_recursive(const path* src) {
  path cur_path = path_from_cstr(directory_path_cstr(src));
  sz root_len = 0;
  sz item_idx = 0;

  path_normalize(&cur_path);
  path_remove_trailing_slash(&cur_path);

  if (cstr8_is_empty(cur_path.buf)) {
    return 0;
  }

  root_len = directory_root_length(cur_path.buf);
  for (item_idx = root_len; cur_path.buf[item_idx] != '\0'; item_idx += 1) {
    if (cur_path.buf[item_idx] == '/') {
      c8 saved_chr = cur_path.buf[item_idx];
      cur_path.buf[item_idx] = '\0';
      if (!cstr8_is_empty(cur_path.buf) && !directory_exists(&cur_path) && !directory_create(&cur_path)) {
        cur_path.buf[item_idx] = saved_chr;
        return 0;
      }
      cur_path.buf[item_idx] = saved_chr;
    }
  }

  if (!directory_exists(&cur_path) && !directory_create(&cur_path)) {
    return 0;
  }

  return 1;
}

func b32 directory_remove_recursive(const path* src) {
  directory_remove_state state;

  if (!directory_exists(src)) {
    return 0;
  }

  memset(&state, 0, size_of(state));
  state.success = 1;

  if (!SDL_EnumerateDirectory(directory_path_cstr(src), directory_remove_callback, &state)) {
    return 0;
  }

  if (!state.success) {
    return 0;
  }

  return SDL_RemovePath(directory_path_cstr(src)) ? 1 : 0;
}

func b32 directory_iterate(const path* src, directory_iterate_callback* callback, void* user_data) {
  directory_iterate_state state;

  if (!directory_exists(src)) {
    return 0;
  }

  memset(&state, 0, size_of(state));
  state.root_path = *src;
  state.recursive = 0;
  state.callback = callback;
  state.user_data = user_data;
  state.success = 1;

  if (!SDL_EnumerateDirectory(directory_path_cstr(src), directory_enumerate_callback, &state)) {
    return state.stop_requested ? 1 : 0;
  }

  return state.success;
}

func b32 directory_iterate_recursive(
    const path* src,
    directory_iterate_callback* callback,
    void* user_data) {
  directory_iterate_state state;

  if (!directory_exists(src)) {
    return 0;
  }

  memset(&state, 0, size_of(state));
  state.root_path = *src;
  state.recursive = 1;
  state.callback = callback;
  state.user_data = user_data;
  state.success = 1;

  if (!SDL_EnumerateDirectory(directory_path_cstr(src), directory_enumerate_callback, &state)) {
    return state.stop_requested ? 1 : 0;
  }

  return state.success;
}

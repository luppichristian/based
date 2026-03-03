// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/file_stream.h"

#include "filesystem/archive.h"
#include "memory/buffer.h"

#include "../sdl3_include.h"

#include <stdlib.h>
#include <string.h>

func file_stream file_stream_empty(void) {
  file_stream stm;
  memset(&stm, 0, size_of(stm));
  stm.archive_path = path_from_cstr("");
  return stm;
}

func const c8* file_stream_mode_string(u32 mode_flags) {
  if ((mode_flags & FILE_STREAM_OPEN_APPEND) != 0) {
    return (mode_flags & FILE_STREAM_OPEN_READ) != 0 ? "a+b" : "ab";
  }

  if ((mode_flags & FILE_STREAM_OPEN_WRITE) != 0) {
    if ((mode_flags & FILE_STREAM_OPEN_READ) != 0) {
      if ((mode_flags & FILE_STREAM_OPEN_TRUNCATE) != 0) {
        return "w+b";
      }
      return "r+b";
    }

    return "wb";
  }

  return "rb";
}

func sz file_stream_native_size(SDL_IOStream* file_ptr) {
  Sint64 size_value = 0;

  if (file_ptr == NULL) {
    return 0;
  }

  size_value = SDL_GetIOSize(file_ptr);
  if (size_value < 0) {
    return 0;
  }

  return (sz)size_value;
}

func path file_stream_normalize_archive_path(const path* src) {
  path item_path = path_from_cstr(src != NULL ? src->buf : "");
  path_normalize(&item_path);
  path_remove_trailing_slash(&item_path);
  return item_path;
}

func archive_entry* file_stream_find_archive_entry(archive* arc, const path* src) {
  path item_path = file_stream_normalize_archive_path(src);
  sz item_idx = 0;

  if (arc == NULL) {
    return NULL;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    path ent_path = file_stream_normalize_archive_path(&arc->entries[item_idx].item_path);
    if (cstr8_cmp(ent_path.buf, item_path.buf) == 0) {
      return &arc->entries[item_idx];
    }
  }

  return NULL;
}

func b32 file_stream_reserve_memory(file_stream* stm, sz min_capacity) {
  u8* new_ptr = NULL;
  sz new_capacity = 0;

  if (stm->memory_capacity >= min_capacity) {
    return 1;
  }

  new_capacity = stm->memory_capacity == 0 ? 64 : stm->memory_capacity;
  while (new_capacity < min_capacity) {
    if (new_capacity > SZ_MAX / 2) {
      new_capacity = min_capacity;
      break;
    }
    new_capacity *= 2;
  }

  new_ptr = (u8*)realloc(stm->memory_ptr, (size_t)new_capacity);
  if (new_ptr == NULL) {
    return 0;
  }

  stm->memory_ptr = new_ptr;
  stm->memory_capacity = new_capacity;
  return 1;
}

func file_stream file_stream_open(const path* src, u32 mode_flags) {
  file_stream stm = file_stream_empty();
  SDL_IOStream* file_ptr = NULL;
  const c8* mode_str = file_stream_mode_string(mode_flags);

  file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", mode_str);
  if (file_ptr == NULL && (mode_flags & FILE_STREAM_OPEN_CREATE) != 0 &&
      (mode_flags & FILE_STREAM_OPEN_APPEND) == 0 &&
      (mode_flags & FILE_STREAM_OPEN_READ) != 0 &&
      (mode_flags & FILE_STREAM_OPEN_WRITE) != 0) {
    file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", "w+b");
  }

  if (file_ptr == NULL) {
    return stm;
  }

  stm.kind = FILE_STREAM_KIND_NATIVE;
  stm.mode_flags = mode_flags;
  stm.native_handle = file_ptr;
  return stm;
}

func file_stream file_stream_open_archive(archive* arc, const path* src, u32 mode_flags) {
  file_stream stm = file_stream_empty();
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL) {
    return stm;
  }

  ent = file_stream_find_archive_entry(arc, src);
  if (ent == NULL && (mode_flags & FILE_STREAM_OPEN_WRITE) == 0 &&
      (mode_flags & FILE_STREAM_OPEN_APPEND) == 0) {
    return stm;
  }

  stm.kind = FILE_STREAM_KIND_ARCHIVE;
  stm.mode_flags = mode_flags;
  stm.archive_ref = arc;
  stm.archive_path = file_stream_normalize_archive_path(src);

  if (ent != NULL && !ent->is_directory && ent->data_size > 0) {
    if (!file_stream_reserve_memory(&stm, ent->data_size)) {
      file_stream_close(&stm);
      return file_stream_empty();
    }

    memcpy(stm.memory_ptr, ent->data_ptr, ent->data_size);
    stm.memory_size = ent->data_size;
  }

  if ((mode_flags & FILE_STREAM_OPEN_TRUNCATE) != 0) {
    stm.memory_size = 0;
  }

  if ((mode_flags & FILE_STREAM_OPEN_APPEND) != 0) {
    stm.cursor = stm.memory_size;
  }

  return stm;
}

func void file_stream_close(file_stream* stm) {
  SDL_IOStream* file_ptr = NULL;

  if (stm == NULL) {
    return;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    file_ptr = (SDL_IOStream*)stm->native_handle;
    if (file_ptr != NULL) {
      SDL_CloseIO(file_ptr);
    }
  } else if (stm->kind == FILE_STREAM_KIND_ARCHIVE) {
    if (stm->dirty && stm->archive_ref != NULL) {
      buffer data = buffer_from(stm->memory_ptr, stm->memory_size);
      (void)archive_write_all(stm->archive_ref, &stm->archive_path, data);
    }

    free(stm->memory_ptr);
  }

  *stm = file_stream_empty();
}

func b32 file_stream_is_open(const file_stream* stm) {
  if (stm == NULL) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    return stm->native_handle != NULL ? 1 : 0;
  }

  if (stm->kind == FILE_STREAM_KIND_ARCHIVE) {
    return stm->archive_ref != NULL ? 1 : 0;
  }

  return 0;
}

func sz file_stream_read(file_stream* stm, void* dst, sz size) {
  sz available = 0;
  sz read_size = 0;

  if (!file_stream_is_open(stm) || dst == NULL || size == 0) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    return (sz)SDL_ReadIO((SDL_IOStream*)stm->native_handle, dst, (size_t)size);
  }

  if ((stm->mode_flags & FILE_STREAM_OPEN_READ) == 0) {
    return 0;
  }

  if (stm->cursor >= stm->memory_size) {
    return 0;
  }

  available = stm->memory_size - stm->cursor;
  read_size = size < available ? size : available;
  memcpy(dst, stm->memory_ptr + stm->cursor, read_size);
  stm->cursor += read_size;
  return read_size;
}

func sz file_stream_write(file_stream* stm, const void* src, sz size) {
  sz end_pos = 0;

  if (!file_stream_is_open(stm) || (src == NULL && size > 0) || size == 0) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    return (sz)SDL_WriteIO((SDL_IOStream*)stm->native_handle, src, (size_t)size);
  }

  if ((stm->mode_flags & FILE_STREAM_OPEN_WRITE) == 0 &&
      (stm->mode_flags & FILE_STREAM_OPEN_APPEND) == 0) {
    return 0;
  }

  if ((stm->mode_flags & FILE_STREAM_OPEN_APPEND) != 0) {
    stm->cursor = stm->memory_size;
  }

  end_pos = stm->cursor + size;
  if (!file_stream_reserve_memory(stm, end_pos)) {
    return 0;
  }

  memcpy(stm->memory_ptr + stm->cursor, src, size);
  stm->cursor = end_pos;
  if (stm->memory_size < end_pos) {
    stm->memory_size = end_pos;
  }
  stm->dirty = 1;
  return size;
}

func b32 file_stream_seek(file_stream* stm, i64 offset, file_stream_seek_basis basis) {
  i64 base_pos = 0;
  i64 new_pos = 0;
  SDL_IOWhence io_basis = SDL_IO_SEEK_SET;

  if (!file_stream_is_open(stm)) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    if (basis == FILE_STREAM_SEEK_BASIS_CURRENT) {
      io_basis = SDL_IO_SEEK_CUR;
    } else if (basis == FILE_STREAM_SEEK_BASIS_END) {
      io_basis = SDL_IO_SEEK_END;
    }

    return SDL_SeekIO((SDL_IOStream*)stm->native_handle, (Sint64)offset, io_basis) >= 0 ? 1 : 0;
  }

  if (basis == FILE_STREAM_SEEK_BASIS_CURRENT) {
    base_pos = (i64)stm->cursor;
  } else if (basis == FILE_STREAM_SEEK_BASIS_END) {
    base_pos = (i64)stm->memory_size;
  }

  new_pos = base_pos + offset;
  if (new_pos < 0 || (sz)new_pos > stm->memory_size) {
    return 0;
  }

  stm->cursor = (sz)new_pos;
  return 1;
}

func sz file_stream_tell(const file_stream* stm) {
  Sint64 cur_pos = 0;

  if (!file_stream_is_open(stm)) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    cur_pos = SDL_TellIO((SDL_IOStream*)stm->native_handle);
    if (cur_pos < 0) {
      return 0;
    }
    return (sz)cur_pos;
  }

  return stm->cursor;
}

func sz file_stream_size(const file_stream* stm) {
  if (!file_stream_is_open(stm)) {
    return 0;
  }

  if (stm->kind == FILE_STREAM_KIND_NATIVE) {
    return file_stream_native_size((SDL_IOStream*)stm->native_handle);
  }

  return stm->memory_size;
}

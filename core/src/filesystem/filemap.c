// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/filemap.h"

#include "basic/assert.h"
#include "basic/env_defines.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "filesystem/file.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"
#include "memory/memops.h"

#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

#elif defined(PLATFORM_UNIX)

#  include <fcntl.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>

#endif

global_var thread_local filemap_error filemap_last_error = FILEMAP_ERROR_NONE;

func void filemap_set_error(filemap_error error_code) {
  profile_func_begin;
  filemap_last_error = error_code;
  profile_func_end;
}

func filemap filemap_empty(void) {
  filemap map;
  mem_zero(&map, size_of(map));
  map.source_path = path_from_cstr("");
  return map;
}

func allocator filemap_allocator_resolve(void) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    profile_func_end;
    return alloc;
  }
  profile_func_end;
  return global_get_allocator();
}

func b32 filemap_is_open(const filemap* map) {
  profile_func_begin;
  if (map == NULL) {
    profile_func_end;
    return false;
  }

  if (map->native_file != NULL) {
    profile_func_end;
    return true;
  }

  profile_func_end;
  return map->uses_fallback_copy && map->source_path.buf[0] != '\0' ? true : false;
}

func b32 filemap_is_writable(const filemap* map) {
  b32 result = map != NULL && map->writable != 0;
  return result;
}

func void filemap_mark_dirty(filemap* map) {
  profile_func_begin;
  if (map != NULL && map->writable) {
    map->dirty = 1;
  }
  profile_func_end;
}

func filemap_error filemap_get_last_error(void) {
  filemap_error result = filemap_last_error;
  return result;
}

func b32 filemap_flush(filemap* map) {
  profile_func_begin;
  if (!filemap_is_open(map)) {
    thread_log_error("Cannot flush unopened file map");
    filemap_set_error(FILEMAP_ERROR_INVALID_ARGUMENT);
    profile_func_end;
    return false;
  }

  if (!map->writable) {
    filemap_set_error(FILEMAP_ERROR_NONE);
    profile_func_end;
    return true;
  }

  if (!map->dirty) {
    filemap_set_error(FILEMAP_ERROR_NONE);
    profile_func_end;
    return true;
  }
  assert(map->source_path.buf[0] != '\0');

  if (map->uses_fallback_copy) {
    thread_log_warn("Flushing file map through fallback copy path=%s", map->source_path.buf);
    buffer data = buffer_from(map->data_ptr, map->data_size);
    if (!file_write_all(&map->source_path, data)) {
      thread_log_error("Failed to flush fallback file map path=%s", map->source_path.buf);
      filemap_set_error(FILEMAP_ERROR_IO_FAILED);
      profile_func_end;
      return false;
    }
    map->dirty = 0;
    filemap_set_error(FILEMAP_ERROR_NONE);
    profile_func_end;
    return true;
  }

#if defined(PLATFORM_WINDOWS)
  if (map->data_ptr != NULL && map->data_size > 0 && !FlushViewOfFile(map->data_ptr, (SIZE_T)map->data_size)) {
    thread_log_error("Failed to flush mapped view path=%s", map->source_path.buf);
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    profile_func_end;
    return false;
  }

  if (map->native_file != NULL && !FlushFileBuffers((HANDLE)map->native_file)) {
    thread_log_error("Failed to flush mapped file handle path=%s", map->source_path.buf);
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    profile_func_end;
    return false;
  }
  map->dirty = 0;
  filemap_set_error(FILEMAP_ERROR_NONE);
  profile_func_end;
  return true;
#elif defined(PLATFORM_UNIX)
  if (map->data_ptr != NULL && map->data_size > 0) {
    if (msync(map->data_ptr, map->data_size, MS_SYNC) != 0) {
      thread_log_error("Failed to flush mapped file path=%s", map->source_path.buf);
      filemap_set_error(FILEMAP_ERROR_IO_FAILED);
      profile_func_end;
      return false;
    }
  }
  map->dirty = 0;
  filemap_set_error(FILEMAP_ERROR_NONE);
  profile_func_end;
  return true;
#else
  profile_func_end;
  return true;
#endif
}

func void _filemap_close(filemap* map, callsite site) {
  profile_func_begin;
  allocator alloc = filemap_allocator_resolve();
  if (map == NULL) {
    profile_func_end;
    return;
  }
  assert(map->data_size == 0 || map->source_path.buf[0] != '\0');

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
      .object_ptr = map,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("File map close was suspended path=%s", map->source_path.buf);
    profile_func_end;
    return;
  }

  if (map->uses_fallback_copy) {
    if (map->writable) {
      (void)filemap_flush(map);
    }

    if (map->data_ptr != NULL) {
      allocator_dealloc(alloc, map->data_ptr);
    }
    thread_log_trace("Closed fallback file map path=%s", map->source_path.buf);
    *map = filemap_empty();
    profile_func_end;
    return;
  }

#if defined(PLATFORM_WINDOWS)
  if (map->data_ptr != NULL) {
    UnmapViewOfFile(map->data_ptr);
  }

  if (map->native_mapping != NULL) {
    CloseHandle((HANDLE)map->native_mapping);
  }

  if (map->native_file != NULL) {
    CloseHandle((HANDLE)map->native_file);
  }
#elif defined(PLATFORM_UNIX)
  if (map->data_ptr != NULL && map->data_size > 0) {
    munmap(map->data_ptr, map->data_size);
  }

  if (map->native_file != NULL) {
    close((i32)(up)map->native_file - 1);
  }
#endif

  *map = filemap_empty();
  thread_log_trace("Closed mapped file path=%s", map->source_path.buf);
  profile_func_end;
}

func filemap _filemap_open(const path* src, filemap_access access, callsite site) {
  profile_func_begin;
  allocator alloc = filemap_allocator_resolve();
  filemap map = filemap_empty();

  if (src == NULL || !file_exists(src)) {
    thread_log_error("Cannot open file map for missing path=%s", src != NULL ? src->buf : "<null>");
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    profile_func_end;
    return map;
  }
  assert(access == FILEMAP_ACCESS_READ || access == FILEMAP_ACCESS_READ_WRITE || access == FILEMAP_ACCESS_COPY_ON_WRITE);

  map.source_path = *src;
  map.writable = access != FILEMAP_ACCESS_READ ? 1 : 0;
  map.dirty = access == FILEMAP_ACCESS_COPY_ON_WRITE ? 1 : 0;
  filemap_set_error(FILEMAP_ERROR_NONE);

#if defined(PLATFORM_WINDOWS)
  HANDLE file_handle = INVALID_HANDLE_VALUE;
  HANDLE mapping_handle = NULL;
  DWORD desired_access = GENERIC_READ;
  DWORD protect_flags = PAGE_READONLY;
  DWORD view_access = FILE_MAP_READ;
  LARGE_INTEGER file_size;

  if (access == FILEMAP_ACCESS_READ_WRITE) {
    desired_access = GENERIC_READ | GENERIC_WRITE;
    protect_flags = PAGE_READWRITE;
    view_access = FILE_MAP_READ | FILE_MAP_WRITE;
  } else if (access == FILEMAP_ACCESS_COPY_ON_WRITE) {
    protect_flags = PAGE_WRITECOPY;
    view_access = FILE_MAP_COPY;
  }

  file_handle = CreateFileA(
      src->buf,
      desired_access,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    thread_log_error("Failed to open file map path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    profile_func_end;
    return filemap_empty();
  }

  if (!GetFileSizeEx(file_handle, &file_size)) {
    thread_log_error("Failed to query mapped file size path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    CloseHandle(file_handle);
    profile_func_end;
    return filemap_empty();
  }

  map.native_file = file_handle;
  map.data_size = (sz)file_size.QuadPart;
  if (map.data_size == 0) {
    profile_func_end;
    return map;
  }

  mapping_handle = CreateFileMappingA(
      file_handle,
      NULL,
      protect_flags,
      (DWORD)(file_size.QuadPart >> 32),
      (DWORD)(file_size.QuadPart & 0xffffffff),
      NULL);
  if (mapping_handle == NULL) {
    thread_log_error("Failed to create file mapping path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    CloseHandle(file_handle);
    profile_func_end;
    return filemap_empty();
  }

  map.native_mapping = mapping_handle;
  map.data_ptr = MapViewOfFile(mapping_handle, view_access, 0, 0, (SIZE_T)map.data_size);
  if (map.data_ptr == NULL) {
    thread_log_error("Failed to map file view path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    CloseHandle((HANDLE)map.native_mapping);
    CloseHandle((HANDLE)map.native_file);
    profile_func_end;
    return filemap_empty();
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
      .object_ptr = &map,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("File map open was suspended path=%s", src->buf);
    UnmapViewOfFile(map.data_ptr);
    CloseHandle((HANDLE)map.native_mapping);
    CloseHandle((HANDLE)map.native_file);
    profile_func_end;
    return filemap_empty();
  }
  thread_log_trace("Opened Windows file map path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  profile_func_end;
  return map;
#elif defined(PLATFORM_UNIX)
  i32 open_flags = O_RDONLY;
  i32 prot_flags = PROT_READ;
  i32 map_flags = MAP_PRIVATE;
  i32 file_desc = -1;
  struct stat stat_info;

  if (access == FILEMAP_ACCESS_READ_WRITE) {
    open_flags = O_RDWR;
    prot_flags = PROT_READ | PROT_WRITE;
    map_flags = MAP_SHARED;
  } else if (access == FILEMAP_ACCESS_COPY_ON_WRITE) {
    prot_flags = PROT_READ | PROT_WRITE;
    map_flags = MAP_PRIVATE;
  }

  file_desc = open(src->buf, open_flags);
  if (file_desc < 0) {
    thread_log_error("Failed to open file map path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    profile_func_end;
    return filemap_empty();
  }

  if (fstat(file_desc, &stat_info) != 0) {
    thread_log_error("Failed to query mapped file size path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    close(file_desc);
    profile_func_end;
    return filemap_empty();
  }

  map.native_file = (void*)(up)(file_desc + 1);
  map.data_size = (sz)stat_info.st_size;
  if (map.data_size == 0) {
    profile_func_end;
    return map;
  }

  map.data_ptr = mmap(NULL, map.data_size, prot_flags, map_flags, file_desc, 0);
  if (map.data_ptr == MAP_FAILED) {
    thread_log_error("Failed to map file path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    close(file_desc);
    profile_func_end;
    return filemap_empty();
  }

  map.native_mapping = (void*)(up)map_flags;
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
      .object_ptr = &map,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("File map open was suspended path=%s", src->buf);
    munmap(map.data_ptr, map.data_size);
    close((i32)(up)map.native_file - 1);
    profile_func_end;
    return filemap_empty();
  }
  thread_log_trace("Opened Unix file map path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  profile_func_end;
  return map;
#else
  buffer file_data = {0};

  if (!file_read_all(src, &alloc, &file_data)) {
    thread_log_error("Failed to open fallback file map path=%s", src->buf);
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    profile_func_end;
    return filemap_empty();
  }
  map.data_ptr = file_data.ptr;
  map.data_size = file_data.size;
  map.uses_fallback_copy = 1;
  thread_log_warn("Using fallback file map copy path=%s writable=%u", src->buf, (u32)map.writable);
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
      .object_ptr = &map,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Fallback file map open was suspended path=%s", src->buf);
    allocator_dealloc(alloc, map.data_ptr);
    profile_func_end;
    return filemap_empty();
  }
  thread_log_trace("Opened fallback file map path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  profile_func_end;
  return map;
#endif
}

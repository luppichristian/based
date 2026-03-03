// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/info.h"

#include "basic/env_defines.h"
#include "filesystem/directory.h"
#include "filesystem/file.h"

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

func timestamp filesystem_timestamp_from_filetime(FILETIME value) {
  ULARGE_INTEGER raw_value;
  u64 total_microseconds = 0;

  raw_value.LowPart = value.dwLowDateTime;
  raw_value.HighPart = value.dwHighDateTime;
  if (raw_value.QuadPart == 0) {
    return timestamp_zero();
  }

  total_microseconds = raw_value.QuadPart / 10;
  if (total_microseconds < 11644473600000000ULL) {
    return timestamp_zero();
  }

  return timestamp_from_microseconds((i64)(total_microseconds - 11644473600000000ULL));
}

func filesystem_entry_kind filesystem_kind_from_attributes(DWORD attributes) {
  if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
    return FILESYSTEM_ENTRY_KIND_SYMLINK;
  }

  if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
    return FILESYSTEM_ENTRY_KIND_DIRECTORY;
  }

  return FILESYSTEM_ENTRY_KIND_FILE;
}

func void filesystem_info_set_windows_flags(filesystem_info* info, DWORD attributes) {
  info->is_read_only = (attributes & FILE_ATTRIBUTE_READONLY) != 0 ? 1 : 0;
  info->hidden = (attributes & FILE_ATTRIBUTE_HIDDEN) != 0 ? 1 : 0;
}

#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)

#  include <sys/stat.h>
#  include <unistd.h>

func const c8* filesystem_name_ptr(const c8* src) {
  const c8* last_sep = src;
  sz item_idx = 0;

  if (src == NULL) {
    return "";
  }

  for (item_idx = 0; src[item_idx] != '\0'; item_idx += 1) {
    if (src[item_idx] == '/' || src[item_idx] == '\\') {
      last_sep = src + item_idx + 1;
    }
  }

  return last_sep;
}

func filesystem_entry_kind filesystem_kind_from_mode(mode_t mode_value) {
  if (S_ISLNK(mode_value)) {
    return FILESYSTEM_ENTRY_KIND_SYMLINK;
  }

  if (S_ISDIR(mode_value)) {
    return FILESYSTEM_ENTRY_KIND_DIRECTORY;
  }

  if (S_ISREG(mode_value)) {
    return FILESYSTEM_ENTRY_KIND_FILE;
  }

  return FILESYSTEM_ENTRY_KIND_OTHER;
}

func timestamp filesystem_timestamp_from_timespec(struct timespec value) {
  i64 microseconds = (i64)value.tv_sec * 1000000;
  microseconds += (i64)(value.tv_nsec / 1000);
  return timestamp_from_microseconds(microseconds);
}

#else

func const c8* filesystem_name_ptr(const c8* src) {
  const c8* last_sep = src;
  sz item_idx = 0;

  if (src == NULL) {
    return "";
  }

  for (item_idx = 0; src[item_idx] != '\0'; item_idx += 1) {
    if (src[item_idx] == '/' || src[item_idx] == '\\') {
      last_sep = src + item_idx + 1;
    }
  }

  return last_sep;
}

#endif

func filesystem_info filesystem_info_empty(void) {
  filesystem_info info;

  info.kind = FILESYSTEM_ENTRY_KIND_NONE;
  info.size = 0;
  info.create_time = timestamp_zero();
  info.access_time = timestamp_zero();
  info.write_time = timestamp_zero();
  info.exists = 0;
  info.is_read_only = 0;
  info.hidden = 0;
  return info;
}

func b32 filesystem_info_query(const path* src, filesystem_info* out_info) {
  filesystem_info info = filesystem_info_empty();
  const c8* src_str = src != NULL ? src->buf : "";

  if (out_info == NULL || src == NULL || src->buf[0] == '\0') {
    return 0;
  }

#if defined(PLATFORM_WINDOWS)
  WIN32_FILE_ATTRIBUTE_DATA attr_data;

  if (!GetFileAttributesExA(src_str, GetFileExInfoStandard, &attr_data)) {
    return 0;
  }

  info.kind = filesystem_kind_from_attributes(attr_data.dwFileAttributes);
  info.exists = 1;
  info.size = (sz)(((u64)attr_data.nFileSizeHigh << 32) | attr_data.nFileSizeLow);
  info.create_time = filesystem_timestamp_from_filetime(attr_data.ftCreationTime);
  info.access_time = filesystem_timestamp_from_filetime(attr_data.ftLastAccessTime);
  info.write_time = filesystem_timestamp_from_filetime(attr_data.ftLastWriteTime);
  filesystem_info_set_windows_flags(&info, attr_data.dwFileAttributes);
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  struct stat stat_info;
  const c8* name_ptr = NULL;

  if (lstat(src_str, &stat_info) != 0) {
    return 0;
  }

  info.kind = filesystem_kind_from_mode(stat_info.st_mode);
  info.exists = 1;
  info.size = (sz)stat_info.st_size;
#  if defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
  info.access_time = filesystem_timestamp_from_timespec(stat_info.st_atimespec);
  info.write_time = filesystem_timestamp_from_timespec(stat_info.st_mtimespec);
  info.create_time = filesystem_timestamp_from_timespec(stat_info.st_ctimespec);
#  else
  info.access_time = filesystem_timestamp_from_timespec(stat_info.st_atim);
  info.write_time = filesystem_timestamp_from_timespec(stat_info.st_mtim);
  info.create_time = filesystem_timestamp_from_timespec(stat_info.st_ctim);
#  endif
  info.read_only = (stat_info.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0 ? 1 : 0;

  name_ptr = filesystem_name_ptr(src_str);
  info.hidden = (name_ptr[0] == '.' && name_ptr[1] != '\0') ? 1 : 0;
#else
  info.exists = path_exists(src);
  if (!info.exists) {
    return 0;
  }

  if (directory_exists(src)) {
    info.kind = FILESYSTEM_ENTRY_KIND_DIRECTORY;
  } else if (file_exists(src)) {
    info.kind = FILESYSTEM_ENTRY_KIND_FILE;
    (void)file_get_size(src, &info.size);
  } else {
    info.kind = FILESYSTEM_ENTRY_KIND_OTHER;
  }

  info.write_time = path_get_last_write_time(src);
  info.create_time = info.write_time;
  info.access_time = info.write_time;
  info.read_only = 0;
  info.hidden = (filesystem_name_ptr(src_str)[0] == '.') ? 1 : 0;
#endif

  *out_info = info;
  return 1;
}

func b32 filesystem_is_symlink(const path* src) {
  filesystem_info info;

  if (!filesystem_info_query(src, &info)) {
    return 0;
  }

  return info.kind == FILESYSTEM_ENTRY_KIND_SYMLINK ? 1 : 0;
}

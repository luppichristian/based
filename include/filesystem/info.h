// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../utils/timestamp.h"
#include "path.h"

typedef enum filesystem_entry_kind {
  FILESYSTEM_ENTRY_KIND_NONE = 0,
  FILESYSTEM_ENTRY_KIND_FILE = 1,
  FILESYSTEM_ENTRY_KIND_DIRECTORY = 2,
  FILESYSTEM_ENTRY_KIND_SYMLINK = 3,
  FILESYSTEM_ENTRY_KIND_OTHER = 4,
} filesystem_entry_kind;

typedef struct filesystem_info {
  filesystem_entry_kind kind;
  sz size;
  timestamp create_time;
  timestamp access_time;
  timestamp write_time;
  b32 exists;
  b32 is_read_only;
  b32 hidden;
} filesystem_info;

// Queries metadata for src. Returns 1 on success, 0 otherwise.
func b32 filesystem_info_query(const path* src, filesystem_info* out_info);

// Returns 1 when src exists and refers to a symbolic link, 0 otherwise.
func b32 filesystem_is_symlink(const path* src);

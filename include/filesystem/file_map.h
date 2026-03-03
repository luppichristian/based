// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef enum file_map_access {
  FILE_MAP_ACCESS_READ = 1,
  FILE_MAP_ACCESS_READ_WRITE = 2,
  FILE_MAP_ACCESS_COPY_ON_WRITE = 3,
} file_map_access;

typedef struct file_map {
  void* data_ptr;
  sz data_size;
  void* native_file;
  void* native_mapping;
  b32 writable;
  b32 dirty;
  b32 uses_fallback_copy;
  path source_path;
} file_map;

// Opens src as a mapped file view. Returns an empty map on failure.
func file_map file_map_open(const path* src, file_map_access access);

// Returns 1 when map currently references an open mapping.
func b32 file_map_is_open(const file_map* map);

// Flushes pending writes back to the underlying file. Returns 1 on success, 0 otherwise.
func b32 file_map_flush(file_map* map);

// Releases the mapping and resets map to an empty value.
func void file_map_close(file_map* map);

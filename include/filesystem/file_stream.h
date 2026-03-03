// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/utility_defines.h"
#include "path.h"

typedef struct archive archive;

typedef enum file_stream_kind {
  FILE_STREAM_KIND_NONE = 0,
  FILE_STREAM_KIND_NATIVE = 1,
  FILE_STREAM_KIND_ARCHIVE = 2,
} file_stream_kind;

typedef enum file_stream_seek_basis {
  FILE_STREAM_SEEK_BASIS_BEGIN = 0,
  FILE_STREAM_SEEK_BASIS_CURRENT = 1,
  FILE_STREAM_SEEK_BASIS_END = 2,
} file_stream_seek_basis;

enum {
  FILE_STREAM_OPEN_READ = bit(0),
  FILE_STREAM_OPEN_WRITE = bit(1),
  FILE_STREAM_OPEN_APPEND = bit(2),
  FILE_STREAM_OPEN_CREATE = bit(3),
  FILE_STREAM_OPEN_TRUNCATE = bit(4),
};

typedef struct file_stream {
  file_stream_kind kind;
  u32 mode_flags;
  void* native_handle;
  archive* archive_ref;
  path archive_path;
  u8* memory_ptr;
  sz memory_size;
  sz memory_capacity;
  sz cursor;
  b32 dirty;
} file_stream;

// Opens a native filesystem stream from src.
func file_stream file_stream_open(const path* src, u32 mode_flags);

// Opens a stream that targets a file entry inside arc.
func file_stream file_stream_open_archive(archive* arc, const path* src, u32 mode_flags);

// Flushes pending archive writes and releases the stream.
func void file_stream_close(file_stream* stm);

// Returns 1 when stm currently references an open stream.
func b32 file_stream_is_open(const file_stream* stm);

// Reads up to size bytes into dst. Returns the number of bytes read.
func sz file_stream_read(file_stream* stm, void* dst, sz size);

// Writes up to size bytes from src. Returns the number of bytes written.
func sz file_stream_write(file_stream* stm, const void* src, sz size);

// Seeks the current cursor. Returns 1 on success, 0 otherwise.
func b32 file_stream_seek(file_stream* stm, i64 offset, file_stream_seek_basis basis);

// Returns the current cursor position.
func sz file_stream_tell(const file_stream* stm);

// Returns the logical byte size of the underlying stream.
func sz file_stream_size(const file_stream* stm);

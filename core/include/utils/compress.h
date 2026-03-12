// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../memory/buffer.h"

// =========================================================================
c_begin;
// =========================================================================

typedef enum compress_error {
  COMPRESS_ERROR_NONE = 0,
  COMPRESS_ERROR_INVALID_ARGUMENT,
  COMPRESS_ERROR_ALLOCATION_FAILED,
  COMPRESS_ERROR_CORRUPT_DATA,
} compress_error;

// Returns a stable string label for a compression error code.
func cstr8 compress_error_to_cstr(compress_error error);

// Compresses a byte buffer into the module's Huffman-based wire format.
// The output buffer is allocated with alloc and must be released with compress_free.
func compress_error compress_encode(buffer src, allocator alloc, buffer* out_compressed);

// Decompresses a buffer produced by compress_encode.
// The output buffer is allocated with alloc and must be released with compress_free.
func compress_error compress_decode(buffer src, allocator alloc, buffer* out_decompressed);

// =========================================================================
c_end;
// =========================================================================

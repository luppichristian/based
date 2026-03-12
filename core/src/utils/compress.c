// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/compress.h"

#include "basic/profiler.h"
#include "basic/safe.h"
#include "context/thread_ctx.h"
#include "memory/memops.h"
#include "utils/huffman.h"

#define COMPRESS_MAGIC_0          'B'
#define COMPRESS_MAGIC_1          'C'
#define COMPRESS_MAGIC_2          'M'
#define COMPRESS_MAGIC_3          '1'
#define COMPRESS_HEADER_BASE_SIZE 14U

func void compress_write_u16_le(u8* dst, u16 value) {
  dst[0] = (u8)(value & 0xFFU);
  dst[1] = (u8)((value >> 8U) & 0xFFU);
}

func void compress_write_u64_le(u8* dst, u64 value) {
  safe_for (sz idx = 0; idx < 8; idx++) {
    dst[idx] = (u8)((value >> (idx * 8)) & 0xFFULL);
  }
}

func u16 compress_read_u16_le(const u8* src) {
  return (u16)src[0] | (u16)((u16)src[1] << 8U);
}

func u64 compress_read_u64_le(const u8* src) {
  u64 value = 0;
  safe_for (sz idx = 0; idx < 8; idx++) {
    value |= (u64)src[idx] << (idx * 8);
  }
  return value;
}

func sz compress_count_active_symbols(const huffman_tree* tree) {
  sz active_count = 0;
  if (tree == NULL) {
    return 0;
  }

  safe_for (sz sym_idx = 0; sym_idx < HUFFMAN_SYMBOL_CAP; sym_idx++) {
    if (tree->symbol_frequencies[sym_idx] != 0) {
      active_count++;
    }
  }
  return active_count;
}

func compress_error compress_validate_allocator(allocator alloc, cstr8 func_name) {
  if (alloc.alloc_fn == NULL || alloc.dealloc_fn == NULL) {
    thread_log_error("%s: Allocator is missing required callbacks", func_name);
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  return COMPRESS_ERROR_NONE;
}

func cstr8 compress_error_to_cstr(compress_error error) {
  profile_func_begin;
  cstr8 text = "UNKNOWN";
  switch (error) {
    case COMPRESS_ERROR_NONE:
      text = "NONE";
      break;
    case COMPRESS_ERROR_INVALID_ARGUMENT:
      text = "INVALID_ARGUMENT";
      break;
    case COMPRESS_ERROR_ALLOCATION_FAILED:
      text = "ALLOCATION_FAILED";
      break;
    case COMPRESS_ERROR_CORRUPT_DATA:
      text = "CORRUPT_DATA";
      break;
    default:
      break;
  }
  profile_func_end;
  return text;
}

func compress_error compress_encode(buffer src, allocator alloc, buffer* out_compressed) {
  profile_func_begin;
  if (out_compressed == NULL) {
    thread_log_error("compress_encode: Output buffer is null");
    profile_func_end;
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  *out_compressed = (buffer) {0};
  compress_error alloc_check = compress_validate_allocator(alloc, "compress_encode");
  if (alloc_check != COMPRESS_ERROR_NONE) {
    profile_func_end;
    return alloc_check;
  }

  if (src.size > 0 && src.ptr == NULL) {
    thread_log_error("compress_encode: Source pointer is null for non-empty buffer");
    profile_func_end;
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  huffman_tree tree = {0};
  if (!huffman_tree_build_for_bytes(&tree, src)) {
    thread_log_error("compress_encode: Failed to build Huffman tree");
    profile_func_end;
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  sz active_count = compress_count_active_symbols(&tree);
  u64 payload_bits = huffman_tree_bit_length_for_bytes(&tree, src);
  sz payload_bytes = (sz)((payload_bits + 7ULL) / 8ULL);
  sz header_size = (sz)COMPRESS_HEADER_BASE_SIZE + active_count * 9U;
  sz total_size = header_size + payload_bytes;
  u8* out_ptr = (u8*)allocator_calloc(alloc, total_size == 0 ? 1 : total_size, 1);
  if (out_ptr == NULL) {
    thread_log_error("compress_encode: Failed to allocate %zu bytes", total_size);
    profile_func_end;
    return COMPRESS_ERROR_ALLOCATION_FAILED;
  }

  out_ptr[0] = COMPRESS_MAGIC_0;
  out_ptr[1] = COMPRESS_MAGIC_1;
  out_ptr[2] = COMPRESS_MAGIC_2;
  out_ptr[3] = COMPRESS_MAGIC_3;
  compress_write_u64_le(out_ptr + 4, (u64)src.size);
  compress_write_u16_le(out_ptr + 12, (u16)active_count);

  sz write_offset = COMPRESS_HEADER_BASE_SIZE;
  safe_for (sz sym_idx = 0; sym_idx < HUFFMAN_SYMBOL_CAP; sym_idx++) {
    if (tree.symbol_frequencies[sym_idx] == 0) {
      continue;
    }

    out_ptr[write_offset++] = (u8)sym_idx;
    compress_write_u64_le(out_ptr + write_offset, tree.symbol_frequencies[sym_idx]);
    write_offset += 8;
  }

  const u8* src_bytes = (const u8*)src.ptr;
  sz bit_offset = 0;
  safe_for (sz idx = 0; idx < src.size; idx++) {
    huffman_code code = huffman_tree_get_code(&tree, src_bytes[idx]);
    safe_for (sz bit_idx = 0; bit_idx < code.bit_length; bit_idx++) {
      if (huffman_code_get_bit(&code, bit_idx) != 0) {
        sz byte_idx = write_offset + (bit_offset / 8);
        sz byte_bit = bit_offset % 8;
        out_ptr[byte_idx] |= (u8)(1U << byte_bit);
      }
      bit_offset++;
    }
  }

  out_compressed->ptr = out_ptr;
  out_compressed->size = total_size;
  thread_log_info(
      "compress_encode: Encoded %zu bytes into %zu bytes with %zu active symbols",
      src.size,
      total_size,
      active_count);
  profile_func_end;
  return COMPRESS_ERROR_NONE;
}

func compress_error compress_decode(buffer src, allocator alloc, buffer* out_decompressed) {
  profile_func_begin;
  if (out_decompressed == NULL) {
    thread_log_error("compress_decode: Output buffer is null");
    profile_func_end;
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  *out_decompressed = (buffer) {0};
  compress_error alloc_check = compress_validate_allocator(alloc, "compress_decode");
  if (alloc_check != COMPRESS_ERROR_NONE) {
    profile_func_end;
    return alloc_check;
  }

  if (src.size > 0 && src.ptr == NULL) {
    thread_log_error("compress_decode: Source pointer is null for non-empty buffer");
    profile_func_end;
    return COMPRESS_ERROR_INVALID_ARGUMENT;
  }

  if (src.size < COMPRESS_HEADER_BASE_SIZE) {
    thread_log_error("compress_decode: Source is too small to contain a header");
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  const u8* src_bytes = (const u8*)src.ptr;
  if (src_bytes[0] != COMPRESS_MAGIC_0 || src_bytes[1] != COMPRESS_MAGIC_1 ||
      src_bytes[2] != COMPRESS_MAGIC_2 || src_bytes[3] != COMPRESS_MAGIC_3) {
    thread_log_error("compress_decode: Invalid magic header");
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  u64 output_size_u64 = compress_read_u64_le(src_bytes + 4);
  u16 active_count = compress_read_u16_le(src_bytes + 12);
  if (active_count > HUFFMAN_SYMBOL_CAP) {
    thread_log_error("compress_decode: Active symbol count %u exceeds %u", active_count, HUFFMAN_SYMBOL_CAP);
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }
  if (output_size_u64 > (u64)SZ_MAX) {
    thread_log_error("compress_decode: Output size %llu exceeds sz range", output_size_u64);
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  sz header_size = (sz)COMPRESS_HEADER_BASE_SIZE + (sz)active_count * 9U;
  if (src.size < header_size) {
    thread_log_error("compress_decode: Source is smaller than the declared header");
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  u64 frequencies[HUFFMAN_SYMBOL_CAP] = {0};
  u64 frequency_sum = 0;
  sz read_offset = COMPRESS_HEADER_BASE_SIZE;
  safe_for (sz entry_idx = 0; entry_idx < active_count; entry_idx++) {
    u8 symbol = src_bytes[read_offset++];
    u64 frequency = compress_read_u64_le(src_bytes + read_offset);
    read_offset += 8;
    if (frequency == 0 || frequencies[symbol] != 0) {
      thread_log_error("compress_decode: Header contains invalid symbol frequency data");
      profile_func_end;
      return COMPRESS_ERROR_CORRUPT_DATA;
    }

    frequencies[symbol] = frequency;
    frequency_sum += frequency;
  }

  if (frequency_sum != output_size_u64) {
    thread_log_error(
        "compress_decode: Frequency sum %llu does not match output size %llu",
        frequency_sum,
        output_size_u64);
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  huffman_tree tree = {0};
  if (!huffman_tree_build(&tree, frequencies, HUFFMAN_SYMBOL_CAP)) {
    thread_log_error("compress_decode: Failed to rebuild Huffman tree");
    profile_func_end;
    return COMPRESS_ERROR_CORRUPT_DATA;
  }

  sz output_size = (sz)output_size_u64;
  u8* out_ptr = (u8*)allocator_alloc(alloc, output_size == 0 ? 1 : output_size);
  if (out_ptr == NULL) {
    thread_log_error("compress_decode: Failed to allocate %zu bytes", output_size);
    profile_func_end;
    return COMPRESS_ERROR_ALLOCATION_FAILED;
  }

  if (output_size == 0) {
    out_decompressed->ptr = out_ptr;
    out_decompressed->size = 0;
    profile_func_end;
    return COMPRESS_ERROR_NONE;
  }

  if (tree.symbol_count == 1) {
    mem_set8(out_ptr, (u8)tree.root_symbol, output_size);
    out_decompressed->ptr = out_ptr;
    out_decompressed->size = output_size;
    profile_func_end;
    return COMPRESS_ERROR_NONE;
  }

  buffer bitstream = buffer_from((void*)(src_bytes + header_size), src.size - header_size);
  sz next_bit_offset = 0;
  safe_for (sz out_idx = 0; out_idx < output_size; out_idx++) {
    if (!huffman_tree_decode_symbol(&tree, bitstream, next_bit_offset, &out_ptr[out_idx], &next_bit_offset)) {
      allocator_dealloc(alloc, out_ptr);
      thread_log_error("compress_decode: Failed to decode symbol at index %zu", out_idx);
      profile_func_end;
      return COMPRESS_ERROR_CORRUPT_DATA;
    }
  }

  out_decompressed->ptr = out_ptr;
  out_decompressed->size = output_size;
  thread_log_info(
      "compress_decode: Decoded %zu bytes from %zu bytes of compressed data",
      output_size,
      src.size);
  profile_func_end;
  return COMPRESS_ERROR_NONE;
}
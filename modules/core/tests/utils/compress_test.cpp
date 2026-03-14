// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_compress_test, encode_decode_round_trip_text) {
  const u8 src_bytes[] = "the quick brown fox jumps over the lazy dog";
  buffer src = buffer_from((void*)src_bytes, size_of(src_bytes) - 1);
  allocator alloc = thread_get_allocator();
  buffer compressed = {};
  buffer decompressed = {};

  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_encode(src, alloc, &compressed));
  ASSERT_GT(compressed.size, 0U);
  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_decode(compressed, alloc, &decompressed));
  ASSERT_EQ(src.size, decompressed.size);
  EXPECT_TRUE(mem_cmp(src.ptr, decompressed.ptr, src.size) != 0);

  allocator_dealloc(alloc, decompressed.ptr);
  allocator_dealloc(alloc, compressed.ptr);
}

TEST(utils_compress_test, encode_decode_round_trip_binary_data) {
  u8 src_bytes[256] = {};
  safe_for (sz idx = 0; idx < size_of(src_bytes); ++idx) {
    src_bytes[idx] = (u8)idx;
  }

  allocator alloc = thread_get_allocator();
  buffer compressed = {};
  buffer decompressed = {};

  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_encode(buffer_from(src_bytes, size_of(src_bytes)), alloc, &compressed));
  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_decode(compressed, alloc, &decompressed));
  ASSERT_EQ(size_of(src_bytes), decompressed.size);
  EXPECT_TRUE(mem_cmp(src_bytes, decompressed.ptr, size_of(src_bytes)) != 0);

  allocator_dealloc(alloc, decompressed.ptr);
  allocator_dealloc(alloc, compressed.ptr);
}

TEST(utils_compress_test, empty_buffers_round_trip) {
  allocator alloc = thread_get_allocator();
  buffer compressed = {};
  buffer decompressed = {};

  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_encode(buffer_from(nullptr, 0), alloc, &compressed));
  ASSERT_EQ(COMPRESS_ERROR_NONE, compress_decode(compressed, alloc, &decompressed));
  EXPECT_EQ(0U, decompressed.size);

  allocator_dealloc(alloc, decompressed.ptr);
  allocator_dealloc(alloc, compressed.ptr);
}

TEST(utils_compress_test, decode_rejects_corrupt_payload) {
  allocator alloc = thread_get_allocator();
  u8 bad_bytes[] = {'B', 'C', 'M', '1', 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  buffer decompressed = {};

  EXPECT_EQ(COMPRESS_ERROR_CORRUPT_DATA, compress_decode(buffer_from(bad_bytes, size_of(bad_bytes)), alloc, &decompressed));
}

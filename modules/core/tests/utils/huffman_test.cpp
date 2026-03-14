// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  void encode_bytes_with_tree(const huffman_tree& tree, buffer src, buffer* out_bits, u8* storage, sz storage_size) {
    ASSERT_NE(out_bits, nullptr);
    ASSERT_NE(storage, nullptr);

    mem_zero(storage, storage_size);
    u64 total_bits = huffman_tree_bit_length_for_bytes(&tree, src);
    ASSERT_LE((total_bits + 7ULL) / 8ULL, storage_size);

    const u8* src_bytes = static_cast<const u8*>(src.ptr);
    sz bit_offset = 0;
    for (sz idx = 0; idx < src.size; ++idx) {
      huffman_code code = huffman_tree_get_code(&tree, src_bytes[idx]);
      for (sz bit_idx = 0; bit_idx < code.bit_length; ++bit_idx) {
        if (huffman_code_get_bit(&code, bit_idx) != 0) {
          storage[bit_offset / 8] |= static_cast<u8>(1U << (bit_offset % 8));
        }
        ++bit_offset;
      }
    }

    *out_bits = buffer_from(storage, static_cast<sz>((total_bits + 7ULL) / 8ULL));
  }

}  // namespace

TEST(utils_huffman_test, build_for_bytes_tracks_symbols_and_codes) {
  u8 src_bytes[] = {'b', 'a', 'n', 'a', 'n', 'a'};
  huffman_tree tree = {};

  ASSERT_TRUE(huffman_tree_build_for_bytes(&tree, buffer_from(src_bytes, size_of(src_bytes))) != 0);
  EXPECT_EQ(3U, tree.symbol_count);
  EXPECT_TRUE(huffman_tree_has_symbol(&tree, 'a') != 0);
  EXPECT_TRUE(huffman_tree_has_symbol(&tree, 'b') != 0);
  EXPECT_TRUE(huffman_tree_has_symbol(&tree, 'n') != 0);
  EXPECT_TRUE(huffman_tree_has_symbol(&tree, 'x') == 0);
  EXPECT_GT(huffman_tree_get_code(&tree, 'a').bit_length, 0U);
  EXPECT_GT(huffman_tree_get_code(&tree, 'b').bit_length, 0U);
  EXPECT_GT(huffman_tree_get_code(&tree, 'n').bit_length, 0U);
}

TEST(utils_huffman_test, encoded_bits_round_trip_through_decoder) {
  u8 src_bytes[] = {'b', 'a', 'n', 'a', 'n', 'a', ' ', 'b', 'r', 'e', 'a', 'd'};
  huffman_tree tree = {};
  ASSERT_TRUE(huffman_tree_build_for_bytes(&tree, buffer_from(src_bytes, size_of(src_bytes))) != 0);

  u8 packed_bits[64] = {};
  buffer bitstream = {};
  encode_bytes_with_tree(tree, buffer_from(src_bytes, size_of(src_bytes)), &bitstream, packed_bits, size_of(packed_bits));

  u8 decoded[size_of(src_bytes)] = {};
  sz next_bit_offset = 0;
  for (sz idx = 0; idx < size_of(src_bytes); ++idx) {
    ASSERT_TRUE(huffman_tree_decode_symbol(&tree, bitstream, next_bit_offset, &decoded[idx], &next_bit_offset) != 0);
  }

  EXPECT_TRUE(mem_cmp(decoded, src_bytes, size_of(src_bytes)) != 0);
}

TEST(utils_huffman_test, single_symbol_tree_decodes_without_consuming_bits) {
  u8 src_bytes[] = {7, 7, 7, 7, 7};
  huffman_tree tree = {};
  ASSERT_TRUE(huffman_tree_build_for_bytes(&tree, buffer_from(src_bytes, size_of(src_bytes))) != 0);
  ASSERT_EQ(1U, tree.symbol_count);

  u8 decoded_symbol = 0;
  sz next_bit_offset = 99;
  ASSERT_TRUE(huffman_tree_decode_symbol(&tree, buffer_from(nullptr, 0), 99, &decoded_symbol, &next_bit_offset) != 0);
  EXPECT_EQ(7U, decoded_symbol);
  EXPECT_EQ(99U, next_bit_offset);
}

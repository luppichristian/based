// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/buffer.h"

#define HUFFMAN_SYMBOL_CAP    256
#define HUFFMAN_CODE_WORD_CAP 4
#define HUFFMAN_NODE_CAP      ((HUFFMAN_SYMBOL_CAP * 2) - 1)
#define HUFFMAN_INVALID_NODE  0xFFFFU

// =========================================================================
c_begin;
// =========================================================================

typedef struct huffman_code {
  u16 bit_length;
  u16 reserved0;
  u32 reserved1;
  u64 bits[HUFFMAN_CODE_WORD_CAP];
} huffman_code;

typedef struct huffman_tree {
  sz symbol_count;
  sz node_count;
  u16 root_idx;
  u16 root_symbol;
  u64 symbol_frequencies[HUFFMAN_SYMBOL_CAP];
  huffman_code codes[HUFFMAN_SYMBOL_CAP];
  u64 node_frequencies[HUFFMAN_NODE_CAP];
  u16 node_left[HUFFMAN_NODE_CAP];
  u16 node_right[HUFFMAN_NODE_CAP];
  u16 node_symbol[HUFFMAN_NODE_CAP];
  b32 node_is_leaf[HUFFMAN_NODE_CAP];
} huffman_tree;

// Clears a Huffman tree back to an empty state.
func void huffman_tree_clear(huffman_tree* tree);

// Builds a Huffman tree from a symbol-frequency table.
// symbol_count may be at most HUFFMAN_SYMBOL_CAP. Zero frequencies are ignored.
func b32 huffman_tree_build(huffman_tree* tree, const u64* frequencies, sz symbol_count);

// Builds a byte-oriented Huffman tree by counting frequencies from the source buffer.
func b32 huffman_tree_build_for_bytes(huffman_tree* tree, buffer src);

// Returns true when the tree contains a code for the specified byte value.
func b32 huffman_tree_has_symbol(const huffman_tree* tree, u8 symbol);

// Returns the Huffman code assigned to the specified byte value.
// If the symbol is absent, a zero-initialized code is returned.
func huffman_code huffman_tree_get_code(const huffman_tree* tree, u8 symbol);

// Returns the bit at the requested pos within a Huffman code.
// Bits are ordered from the root toward the leaf.
func b32 huffman_code_get_bit(const huffman_code* code, sz bit_idx);

// Returns the number of encoded bits required to represent the provided bytes.
// Returns 0 when the tree is missing any referenced symbol.
func u64 huffman_tree_bit_length_for_bytes(const huffman_tree* tree, buffer src);

// Decodes a single symbol from a packed bitstream and advances the bit offset.
// For single-symbol trees, out_next_bit_offset matches bit_offset.
func b32 huffman_tree_decode_symbol(
    const huffman_tree* tree,
    buffer bitstream,
    sz bit_offset,
    u8* out_symbol,
    sz* out_next_bit_offset);

// =========================================================================
c_end;
// =========================================================================

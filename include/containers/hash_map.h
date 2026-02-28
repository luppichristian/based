// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/keyword_defines.h"
#include "memory/allocator.h"

// hash_map is an open-addressing Robin Hood hash table.
//
// Keys are always u64.  The caller is responsible for hashing their own
// keys to u64 using the hash_* utilities below.  Values are void*, so the
// caller casts to/from their concrete type at the call site.
//
// The Robin Hood invariant (slots with shorter probe distances give up their
// position to incoming entries with longer ones) keeps average probe distance
// near-optimal and enables tombstone-free deletion via backward shifting.
//
// The map rehashes automatically when load exceeds 75%.  Capacity is always
// a power of two; the minimum is 16 slots.
//
// Iteration:
//   hash_map_iter it = hash_map_iter_begin();
//   hash_map_slot* slot;
//   while ((slot = hash_map_next(&map, &it)) != nullptr) {
//     use(slot->key, slot->value);
//   }

// =========================================================================
// Types
// =========================================================================

// One slot in the backing array.
typedef struct hash_map_slot {
  u64   key;
  void* value;
  u32   probe_dist;  // distance from the slot's ideal (home) position
  b32   occupied;
} hash_map_slot;

// The hash map handle.
typedef struct hash_map {
  hash_map_slot* slots;
  sz             count;   // number of occupied entries
  sz             cap;     // total slot count, always a power of two
  allocator      alloc;
} hash_map;

// Opaque iterator — holds the current slot index.  Obtain one with
// hash_map_iter_begin() and advance it with hash_map_next().
typedef sz hash_map_iter;

// =========================================================================
// Hash Utilities
// =========================================================================

// hash_u64  — mix a 64-bit integer key (splitmix64 finalizer).
func u64 hash_u64(u64 val);

// hash_ptr  — hash a pointer value.
func u64 hash_ptr(const void* ptr);

// hash_bytes — FNV-1a 64-bit hash over an arbitrary byte buffer.
func u64 hash_bytes(const void* ptr, sz len);

// hash_str  — FNV-1a 64-bit hash over a null-terminated string.
func u64 hash_str(const c8* str);

// =========================================================================
// Lifecycle
// =========================================================================

// hash_map_create — allocate a map with at least cap slots (rounded to the
// next power of two, minimum 16) using the given allocator.
func hash_map hash_map_create(sz cap, allocator alloc);

// hash_map_destroy — free all backing memory; the map must not be used afterwards.
func void hash_map_destroy(hash_map* map);

// hash_map_clear — remove all entries without freeing the backing array.
func void hash_map_clear(hash_map* map);

// =========================================================================
// Operations
// =========================================================================

// hash_map_set — insert or update the entry for key.  Returns 1 on success,
// 0 if a rehash was required but memory allocation failed.
func b32 hash_map_set(hash_map* map, u64 key, void* value);

// hash_map_get — return the value for key, or nullptr if not found.
func void* hash_map_get(hash_map* map, u64 key);

// hash_map_has — return 1 if key exists in the map, 0 otherwise.
func b32 hash_map_has(hash_map* map, u64 key);

// hash_map_remove — remove the entry for key.  Returns 1 if the key was
// found and removed, 0 if the key was not present.
func b32 hash_map_remove(hash_map* map, u64 key);

// =========================================================================
// Iteration
// =========================================================================

// hash_map_iter_begin — return an iterator positioned before the first slot.
func hash_map_iter hash_map_iter_begin(void);

// hash_map_next — advance iter to the next occupied slot and return a pointer
// to it, or nullptr when all slots have been visited.
func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter);

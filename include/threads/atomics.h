// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Atomic Types
// =========================================================================

// All fields must be accessed exclusively through the functions below.
// Direct field access bypasses the atomic guarantees.

typedef struct atomic_i32 {
  i32 val;
} atomic_i32;

typedef struct atomic_u32 {
  u32 val;
} atomic_u32;

// Atomic pointer — a void* that can be exchanged across threads.
typedef struct atomic_ptr {
  void* val;
} atomic_ptr;

// =========================================================================
// atomic_i32
// =========================================================================

// Atomically loads and returns the current value.
func i32 atomic_i32_get(atomic_i32* atom);

// Atomically replaces the value with val and returns the previous value.
func i32 atomic_i32_set(atomic_i32* atom, i32 val);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired);

// Atomically adds delta and returns the value before the addition.
func i32 atomic_i32_add(atomic_i32* atom, i32 delta);

// Atomically subtracts delta and returns the value before the subtraction.
func i32 atomic_i32_sub(atomic_i32* atom, i32 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_i32_eq(atomic_i32* atom, i32 val);
func b32 atomic_i32_neq(atomic_i32* atom, i32 val);
func b32 atomic_i32_lt(atomic_i32* atom, i32 val);
func b32 atomic_i32_gt(atomic_i32* atom, i32 val);
func b32 atomic_i32_lte(atomic_i32* atom, i32 val);
func b32 atomic_i32_gte(atomic_i32* atom, i32 val);

// =========================================================================
// atomic_u32
// =========================================================================

// Atomically loads and returns the current value.
func u32 atomic_u32_get(atomic_u32* atom);

// Atomically replaces the value with val and returns the previous value.
func u32 atomic_u32_set(atomic_u32* atom, u32 val);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired);

// Atomically adds delta and returns the value before the addition.
func u32 atomic_u32_add(atomic_u32* atom, u32 delta);

// Atomically subtracts delta and returns the value before the subtraction.
func u32 atomic_u32_sub(atomic_u32* atom, u32 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_u32_eq(atomic_u32* atom, u32 val);
func b32 atomic_u32_neq(atomic_u32* atom, u32 val);
func b32 atomic_u32_lt(atomic_u32* atom, u32 val);
func b32 atomic_u32_gt(atomic_u32* atom, u32 val);
func b32 atomic_u32_lte(atomic_u32* atom, u32 val);
func b32 atomic_u32_gte(atomic_u32* atom, u32 val);

// =========================================================================
// atomic_ptr
// =========================================================================

// Atomically loads and returns the current pointer.
func void* atomic_ptr_get(atomic_ptr* atom);

// Atomically replaces the pointer with val and returns the previous pointer.
func void* atomic_ptr_set(atomic_ptr* atom, void* val);

// If the current pointer equals *expected, replaces it with desired and returns true.
// On failure, writes the current pointer into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired);

// Compare the current pointer against val (via a single atomic load).
func b32 atomic_ptr_eq(atomic_ptr* atom, void* val);
func b32 atomic_ptr_neq(atomic_ptr* atom, void* val);

// =========================================================================
// Memory Fences
// =========================================================================

// Prevents memory operations after this call from being reordered before it (acquire).
func void atomic_fence_acquire(void);

// Prevents memory operations before this call from being reordered after it (release).
func void atomic_fence_release(void);

// Full sequential-consistency fence — combines acquire and release.
func void atomic_fence(void);

// Emits a CPU pause/yield hint. Use inside spin-wait loops to reduce power and
// improve performance on hyper-threaded CPUs.
func void atomic_pause(void);

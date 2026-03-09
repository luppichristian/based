// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/atomics.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include <stdatomic.h>

// Layout-compatibility assertions:
//   atomic_i32 { i32 val; }  <->  SDL_AtomicInt { int value; }    (i32 == int32_t == int)
//   atomic_u32 { u32 val; }  <->  SDL_AtomicU32 { Uint32 value; } (u32 == uint32_t == Uint32)
//   atomic_i64 { i64 val; }  <->  _Atomic int64_t                 (C11; lock-free on all targets)
//   atomic_u64 { u64 val; }  <->  _Atomic uint64_t                (C11; lock-free on all targets)
// The (void*) intermediate cast is the idiomatic C way to suppress strict-aliasing analysis
// while preserving defined pointer-conversion semantics.
static_assert(size_of(atomic_i32) == size_of(SDL_AtomicInt));
static_assert(size_of(atomic_u32) == size_of(SDL_AtomicU32));
static_assert(size_of(atomic_i64) == size_of(_Atomic int64_t));
static_assert(size_of(atomic_u64) == size_of(_Atomic uint64_t));

func memory_order atomic_map_order(atomic_memory_order order) {
  profile_func_begin;
  switch (order) {
    case ATOMIC_MEMORY_ORDER_RELAXED:
      profile_func_end;
      return memory_order_relaxed;
    case ATOMIC_MEMORY_ORDER_CONSUME:
      profile_func_end;
      return memory_order_consume;
    case ATOMIC_MEMORY_ORDER_ACQUIRE:
      profile_func_end;
      return memory_order_acquire;
    case ATOMIC_MEMORY_ORDER_RELEASE:
      profile_func_end;
      return memory_order_release;
    case ATOMIC_MEMORY_ORDER_ACQ_REL:
      profile_func_end;
      return memory_order_acq_rel;
    case ATOMIC_MEMORY_ORDER_SEQ_CST:
    default:
      profile_func_end;
      return memory_order_seq_cst;
  }
}

// =========================================================================
// atomic_i32
// =========================================================================

func i32 atomic_i32_get(atomic_i32* atom) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
}

func i32 atomic_i32_get_explicit(atomic_i32* atom, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i32 result = (i32)atomic_load_explicit((_Atomic int32_t*)(void*)atom, atomic_map_order(order));
  profile_func_end;
  return result;
}

func i32 atomic_i32_set(atomic_i32* atom, i32 val) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i32)SDL_SetAtomicInt((SDL_AtomicInt*)(void*)atom, (int)val);
}

func i32 atomic_i32_set_explicit(atomic_i32* atom, i32 val, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i32 expected = (i32)atomic_load_explicit((_Atomic int32_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic int32_t*)(void*)atom, (int32_t)val, atomic_map_order(order));
  profile_func_end;
  return expected;
}

func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired) {
  profile_func_begin;
  if (atom == NULL || expected == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicInt((SDL_AtomicInt*)(void*)atom, (int)*expected, (int)desired)) {
    profile_func_end;
    return 1;
  }
  *expected = (i32)SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
  profile_func_end;
  return 0;
}

func i32 atomic_i32_add(atomic_i32* atom, i32 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, (int)delta);
}

func i32 atomic_i32_and(atomic_i32* atom, i32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i32 result = (i32)atomic_fetch_and((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  profile_func_end;
  return result;
}

func i32 atomic_i32_or(atomic_i32* atom, i32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i32 result = (i32)atomic_fetch_or((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  profile_func_end;
  return result;
}

func i32 atomic_i32_xor(atomic_i32* atom, i32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i32 result = (i32)atomic_fetch_xor((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  profile_func_end;
  return result;
}

func i32 atomic_i32_sub(atomic_i32* atom, i32 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, -(int)delta);
}

func b32 atomic_i32_eq(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) == val;
}
func b32 atomic_i32_neq(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) != val;
}
func b32 atomic_i32_lt(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) < val;
}
func b32 atomic_i32_gt(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) > val;
}
func b32 atomic_i32_lte(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) <= val;
}
func b32 atomic_i32_gte(atomic_i32* atom, i32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i32_get(atom) >= val;
}

// =========================================================================
// atomic_u32
// =========================================================================

func u32 atomic_u32_get(atomic_u32* atom) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
}

func u32 atomic_u32_get_explicit(atomic_u32* atom, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u32 result = (u32)atomic_load_explicit((_Atomic uint32_t*)(void*)atom, atomic_map_order(order));
  profile_func_end;
  return result;
}

func u32 atomic_u32_set(atomic_u32* atom, u32 val) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u32)SDL_SetAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)val);
}

func u32 atomic_u32_set_explicit(atomic_u32* atom, u32 val, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u32 expected = (u32)atomic_load_explicit((_Atomic uint32_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic uint32_t*)(void*)atom, (uint32_t)val, atomic_map_order(order));
  profile_func_end;
  return expected;
}

func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired) {
  profile_func_begin;
  if (atom == NULL || expected == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)*expected, (Uint32)desired)) {
    profile_func_end;
    return 1;
  }
  *expected = (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
  profile_func_end;
  return 0;
}

// SDL3 has no SDL_AddAtomicU32 — implement via CAS loop.
func u32 atomic_u32_add(atomic_u32* atom, u32 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  u32 old;
  do {
    old = atomic_u32_get(atom);
  } while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old + delta)));
  profile_func_end;
  return old;
}

func u32 atomic_u32_sub(atomic_u32* atom, u32 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  u32 old;
  do {
    old = atomic_u32_get(atom);
  } while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old - delta)));
  profile_func_end;
  return old;
}

func u32 atomic_u32_and(atomic_u32* atom, u32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u32 result = (u32)atomic_fetch_and((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  profile_func_end;
  return result;
}

func u32 atomic_u32_or(atomic_u32* atom, u32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u32 result = (u32)atomic_fetch_or((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  profile_func_end;
  return result;
}

func u32 atomic_u32_xor(atomic_u32* atom, u32 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u32 result = (u32)atomic_fetch_xor((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  profile_func_end;
  return result;
}

func b32 atomic_u32_eq(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) == val;
}
func b32 atomic_u32_neq(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) != val;
}
func b32 atomic_u32_lt(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) < val;
}
func b32 atomic_u32_gt(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) > val;
}
func b32 atomic_u32_lte(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) <= val;
}
func b32 atomic_u32_gte(atomic_u32* atom, u32 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u32_get(atom) >= val;
}

// =========================================================================
// atomic_i64
// =========================================================================

func i64 atomic_i64_get(atomic_i64* atom) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i64)atomic_load((_Atomic int64_t*)(void*)atom);
}

func i64 atomic_i64_get_explicit(atomic_i64* atom, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i64 result = (i64)atomic_load_explicit((_Atomic int64_t*)(void*)atom, atomic_map_order(order));
  profile_func_end;
  return result;
}

func i64 atomic_i64_set(atomic_i64* atom, i64 val) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i64)atomic_exchange((_Atomic int64_t*)(void*)atom, (int64_t)val);
}

func i64 atomic_i64_set_explicit(atomic_i64* atom, i64 val, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i64 expected = (i64)atomic_load_explicit((_Atomic int64_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic int64_t*)(void*)atom, (int64_t)val, atomic_map_order(order));
  profile_func_end;
  return expected;
}

func b32 atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired) {
  profile_func_begin;
  if (atom == NULL || expected == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  int64_t exp = (int64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic int64_t*)(void*)atom, &exp, (int64_t)desired)) {
    profile_func_end;
    return 1;
  }
  *expected = (i64)exp;
  profile_func_end;
  return 0;
}

func i64 atomic_i64_add(atomic_i64* atom, i64 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i64)atomic_fetch_add((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func i64 atomic_i64_sub(atomic_i64* atom, i64 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (i64)atomic_fetch_sub((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func i64 atomic_i64_and(atomic_i64* atom, i64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i64 result = (i64)atomic_fetch_and((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  profile_func_end;
  return result;
}

func i64 atomic_i64_or(atomic_i64* atom, i64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i64 result = (i64)atomic_fetch_or((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  profile_func_end;
  return result;
}

func i64 atomic_i64_xor(atomic_i64* atom, i64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  i64 result = (i64)atomic_fetch_xor((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  profile_func_end;
  return result;
}

func b32 atomic_i64_eq(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) == val;
}
func b32 atomic_i64_neq(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) != val;
}
func b32 atomic_i64_lt(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) < val;
}
func b32 atomic_i64_gt(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) > val;
}
func b32 atomic_i64_lte(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) <= val;
}
func b32 atomic_i64_gte(atomic_i64* atom, i64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_i64_get(atom) >= val;
}

// =========================================================================
// atomic_u64
// =========================================================================

func u64 atomic_u64_get(atomic_u64* atom) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u64)atomic_load((_Atomic uint64_t*)(void*)atom);
}

func u64 atomic_u64_get_explicit(atomic_u64* atom, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u64 result = (u64)atomic_load_explicit((_Atomic uint64_t*)(void*)atom, atomic_map_order(order));
  profile_func_end;
  return result;
}

func u64 atomic_u64_set(atomic_u64* atom, u64 val) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u64)atomic_exchange((_Atomic uint64_t*)(void*)atom, (uint64_t)val);
}

func u64 atomic_u64_set_explicit(atomic_u64* atom, u64 val, atomic_memory_order order) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u64 expected = (u64)atomic_load_explicit((_Atomic uint64_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic uint64_t*)(void*)atom, (uint64_t)val, atomic_map_order(order));
  profile_func_end;
  return expected;
}

func b32 atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired) {
  profile_func_begin;
  if (atom == NULL || expected == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  uint64_t exp = (uint64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic uint64_t*)(void*)atom, &exp, (uint64_t)desired)) {
    profile_func_end;
    return 1;
  }
  *expected = (u64)exp;
  profile_func_end;
  return 0;
}

func u64 atomic_u64_add(atomic_u64* atom, u64 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u64)atomic_fetch_add((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func u64 atomic_u64_sub(atomic_u64* atom, u64 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  profile_func_end;
  return (u64)atomic_fetch_sub((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func u64 atomic_u64_and(atomic_u64* atom, u64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u64 result = (u64)atomic_fetch_and((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  profile_func_end;
  return result;
}

func u64 atomic_u64_or(atomic_u64* atom, u64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u64 result = (u64)atomic_fetch_or((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  profile_func_end;
  return result;
}

func u64 atomic_u64_xor(atomic_u64* atom, u64 mask) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return 0;
  }
  u64 result = (u64)atomic_fetch_xor((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  profile_func_end;
  return result;
}

func b32 atomic_u64_eq(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) == val;
}
func b32 atomic_u64_neq(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) != val;
}
func b32 atomic_u64_lt(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) < val;
}
func b32 atomic_u64_gt(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) > val;
}
func b32 atomic_u64_lte(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) <= val;
}
func b32 atomic_u64_gte(atomic_u64* atom, u64 val) {
  profile_func_begin;
  profile_func_end;
  return atomic_u64_get(atom) >= val;
}

// =========================================================================
// atomic_ptr
// =========================================================================

func void* atomic_ptr_get(atomic_ptr* atom) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(atom != NULL);
  profile_func_end;
  return SDL_GetAtomicPointer(&atom->val);
}

func void* atomic_ptr_set(atomic_ptr* atom, void* val) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(atom != NULL);
  profile_func_end;
  return SDL_SetAtomicPointer(&atom->val, val);
}

func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired) {
  profile_func_begin;
  if (atom == NULL || expected == NULL) {
    profile_func_end;
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicPointer(&atom->val, *expected, desired)) {
    profile_func_end;
    return 1;
  }
  *expected = SDL_GetAtomicPointer(&atom->val);
  profile_func_end;
  return 0;
}

func b32 atomic_ptr_eq(atomic_ptr* atom, void* val) {
  profile_func_begin;
  profile_func_end;
  return atomic_ptr_get(atom) == val;
}
func b32 atomic_ptr_neq(atomic_ptr* atom, void* val) {
  profile_func_begin;
  profile_func_end;
  return atomic_ptr_get(atom) != val;
}

// =========================================================================
// Memory Fences
// =========================================================================

func void atomic_fence_acquire(void) {
  profile_func_begin;
  SDL_MemoryBarrierAcquireFunction();
  profile_func_end;
}

func void atomic_fence_release(void) {
  profile_func_begin;
  SDL_MemoryBarrierReleaseFunction();
  profile_func_end;
}

func void atomic_fence(void) {
  profile_func_begin;
  SDL_MemoryBarrierReleaseFunction();
  SDL_MemoryBarrierAcquireFunction();
  profile_func_end;
}

func void atomic_pause(void) {
  profile_func_begin;
  SDL_CPUPauseInstruction();
  profile_func_end;
}

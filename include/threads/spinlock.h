// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// A CPU-level spin lock.
// Initialize to {0} for the unlocked state.
// Spinlocks busy-wait and are only appropriate for very short critical sections where
// the overhead of an OS mutex is unjustified. Prefer mutex for longer or blocking operations.
// Spinlocks must NOT be locked recursively.
typedef struct spinlock {
  int state;
} spinlock;

// Acquires the spinlock, busy-spinning until it becomes available.
func void spinlock_lock(spinlock* sl);

// Releases the spinlock.
func void spinlock_unlock(spinlock* sl);

// Tries to acquire the spinlock without spinning.
// Returns true if the lock was acquired, false if it is already held.
func b32 spinlock_trylock(spinlock* sl);

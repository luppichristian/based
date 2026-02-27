// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// Opaque handle to a spinlock.
// Spinlocks busy-wait and are only appropriate for very short critical sections where
// the overhead of an OS mutex is unjustified. Prefer mutex for longer or blocking operations.
// Spinlocks must NOT be locked recursively.
typedef void* spinlock;

// Creates a new spinlock in the unlocked state.
func spinlock spinlock_create();

// Destroys the spinlock and releases its resources.
func void spinlock_destroy(spinlock sl);

// Returns true if the spinlock handle is valid, false otherwise.
func b32 spinlock_is_valid(spinlock sl);

// Acquires the spinlock, busy-spinning until it becomes available.
func void spinlock_lock(spinlock sl);

// Releases the spinlock.
func void spinlock_unlock(spinlock sl);

// Tries to acquire the spinlock without spinning.
// Returns true if the lock was acquired, false if it is already held.
func b32 spinlock_trylock(spinlock sl);

// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// =========================================================================
c_begin;
// =========================================================================

// Opaque handle to a semaphore.
typedef void* semaphore;

// Creates a new semaphore with the given initial count and returns a handle to it.
func semaphore _semaphore_create(u32 initial_count, callsite site);

// Destroys the given semaphore and releases any associated resources.
func b32 _semaphore_destroy(semaphore sem, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define semaphore_create(initial_count) _semaphore_create(initial_count, CALLSITE_HERE)
#define semaphore_destroy(sem)          _semaphore_destroy(sem, CALLSITE_HERE)

// Returns true if the given semaphore handle is valid, false otherwise.
func b32 semaphore_is_valid(semaphore sem);

// Acquires (decrements) the given semaphore.
// If the semaphore's count is greater than zero, this function will decrement the count and return
// immediately. If the count is zero, this function will block until another thread releases the count.
func void semaphore_acquire(semaphore sem);

// Tries to acquire the semaphore without blocking.
// Returns true if the count was positive and was decremented, false if the count is zero.
func b32 semaphore_try_acquire(semaphore sem);

// Waits up to millis milliseconds to acquire the semaphore.
// Returns true if the semaphore was acquired, false if the timeout elapsed.
func b32 semaphore_acquire_timeout(semaphore sem, u32 millis);

// Releases the given semaphore, potentially unblocking a waiting thread.
func void semaphore_release(semaphore sem);

// =========================================================================
c_end;
// =========================================================================

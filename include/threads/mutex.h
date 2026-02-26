// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// Opaque handle to a mutex.
typedef void* mutex;

// Creates a new mutex and returns a handle to it.
func mutex mutex_create();

// Destroys the given mutex and releases any associated resources.
func b32 mutex_destroy(mutex mtx);

// Returns true if the given mutex handle is valid, false otherwise.
func b32 mutex_is_valid(mutex mtx);

// Locks the given mutex.
// If the mutex is already locked by another thread,
// this function will block until the mutex becomes available.
func void mutex_lock(mutex mtx);

// Tries to lock the given mutex without blocking.
// Returns true if the lock was acquired, false if the mutex is already held by another thread.
func b32 mutex_trylock(mutex mtx);

// Unlocks the given mutex.
// The mutex must be locked by the current thread before calling this function.
func void mutex_unlock(mutex mtx);
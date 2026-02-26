// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "thread.h"

// =========================================================================
// Current Thread
// =========================================================================

// Returns the OS-level identifier of the calling thread.
func u64 thread_id(void);

// Sets the scheduling priority of the calling thread.
func b32 thread_set_priority(thread_priority priority);

// Suspends the calling thread for at least millis milliseconds.
func void thread_sleep(u32 millis);

// Yields the calling thread's remaining time slice to the OS scheduler.
func void thread_yield(void);

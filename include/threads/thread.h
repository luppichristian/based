// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Thread Priority
// =========================================================================

// Scheduling priority levels for a thread.
typedef enum thread_priority {
  THREAD_PRIORITY_LOW,
  THREAD_PRIORITY_NORMAL,
  THREAD_PRIORITY_HIGH,
  THREAD_PRIORITY_TIME_CRITICAL,
} thread_priority;

// =========================================================================
// Thread
// =========================================================================

// Opaque handle to a thread.
typedef void* thread;

// Entry-point signature for a thread. Returns an exit code.
typedef i32 (*thread_func)(void* arg);

// Creates a new thread that executes entry(arg) and returns a handle to it.
func thread thread_create(thread_func entry, void* arg);

// Creates a new named thread that executes entry(arg) and returns a handle to it.
// The name is used only for debugging purposes (e.g. visible in debuggers/profilers).
func thread thread_create_named(thread_func entry, void* arg, const c8* name);

// Returns true if the given thread handle is valid, false otherwise.
func b32 thread_is_valid(thread thd);

// Blocks until the thread finishes, then stores its exit code in out_exit_code (may be NULL).
// Invalidates the handle — do not use thd after this call.
func b32 thread_join(thread thd, i32* out_exit_code);

// Detaches the thread so it cleans up automatically on exit.
// Invalidates the handle — do not join or detach thd again after this call.
func void thread_detach(thread thd);

// Returns the OS-level identifier of the given thread.
func u64 thread_get_id(thread thd);

// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "thread.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Thread Group
// =========================================================================

// Entry-point for a thread group member.
// idx is the thread's zero-based position within the group.
// arg is the shared user argument passed to thread_group_create.
typedef i32 (*thread_group_func)(u32 idx, void* arg);

// Opaque handle to a fixed-size group of threads all running the same entry function.
typedef void* thread_group;

// Creates a group of count threads, all executing entry(idx, arg).
// Threads start immediately; idx runs from 0 to count-1.
// main_allocator is forwarded to each worker thread's thread_ctx_init path.
// Returns a valid handle on success, or NULL on failure.
func thread_group _thread_group_create(
    u32 count,
    thread_group_func entry,
    void* arg,
    allocator main_allocator,
    callsite site);

// Like thread_group_create, but each thread is named "<base_name>[<idx>]".
// Names are visible in debuggers and profilers.
func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    allocator main_allocator,
    cstr8 base_name,
    callsite site);

// Destroys the group and releases its internal resources.
// Passing NULL is safe and does nothing.
func b32 _thread_group_destroy(thread_group group, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define thread_group_create(count, entry, arg, main_allocator) \
  _thread_group_create(count, entry, arg, main_allocator, CALLSITE_HERE)
#define thread_group_create_named(count, entry, arg, main_allocator, base_name) \
  _thread_group_create_named(count, entry, arg, main_allocator, base_name, CALLSITE_HERE)
#define thread_group_destroy(group) _thread_group_destroy(group, CALLSITE_HERE)

// Returns true if the group handle is valid, false otherwise.
func b32 thread_group_is_valid(thread_group group);

// Returns the number of threads in the group.
func u32 thread_group_get_count(thread_group group);

// Returns the thread handle at the given idx, or NULL if the idx is out of range.
func thread thread_group_get(thread_group group, u32 idx);

// Blocks until every thread in the group has finished.
// If out_exit_codes is non-NULL it must point to an array of at least thread_group_get_count() i32s;
// each element is written with the corresponding thread's exit code in idx order.
// Returns true if all joins succeeded, false otherwise.
func b32 thread_group_join_all(thread_group group, i32* out_exit_codes);

// Detaches all threads in the group so they clean up automatically on exit.
// Returns true if every detach succeeded, false otherwise.
func b32 thread_group_detach_all(thread_group group);

// =========================================================================
c_end;
// =========================================================================

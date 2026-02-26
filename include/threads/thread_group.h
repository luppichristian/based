// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "thread.h"

// =========================================================================
// Thread Group
// =========================================================================

// Entry-point for a thread group member.
// index is the thread's zero-based position within the group.
// arg   is the shared user argument passed to thread_group_create.
typedef i32 (*thread_group_func)(u32 index, void* arg);

// Per-thread context stored alongside each thread handle. Internal — do not access directly.
typedef struct {
  thread_group_func entry;
  void* arg;
  u32 index;
} thread_group_slot;

// A fixed-size group of threads all running the same entry function.
// Initialize with thread_group_create; zero-initialize to represent an empty/invalid group.
typedef struct {
  thread* threads;
  thread_group_slot* slots;
  u32 count;
} thread_group;

// Initializes group with count threads, all executing entry(index, arg).
// Threads start immediately; index runs from 0 to count-1.
// Returns true on success. On failure the group is left zeroed and no threads are running.
func b32 thread_group_create(thread_group* group, u32 count, thread_group_func entry, void* arg);

// Like thread_group_create, but each thread is named "<base_name>[<index>]".
// Names are visible in debuggers and profilers.
func b32 thread_group_create_named(
    thread_group* group,
    u32 count,
    thread_group_func entry,
    void* arg,
    const c8* base_name);

// Frees internal resources. All threads must have been joined or detached before calling this.
func void thread_group_destroy(thread_group* group);

// Returns true if the group was successfully created and holds live thread handles.
func b32 thread_group_is_valid(thread_group* group);

// Returns the number of threads in the group.
func u32 thread_group_count(thread_group* group);

// Returns the thread handle at the given index, or NULL if the index is out of range.
func thread thread_group_get(thread_group* group, u32 index);

// Blocks until every thread in the group has finished.
// If out_exit_codes is non-NULL it must point to an array of at least thread_group_count() i32s;
// each element is written with the corresponding thread's exit code in index order.
// Returns true if all joins succeeded, false otherwise.
func b32 thread_group_join_all(thread_group* group, i32* out_exit_codes);

// Detaches all threads in the group so they clean up automatically on exit.
func void thread_group_detach_all(thread_group* group);

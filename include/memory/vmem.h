// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Virtual Memory
// =========================================================================

// Low-level OS virtual-memory primitives. These sit below the allocator layer
// and are the natural backing for arena and heap blocks on desktop platforms.
//
// On Windows the implementation uses VirtualAlloc / VirtualFree.
// On POSIX (Linux, macOS, Android, iOS) it uses mmap / mprotect / munmap.
// On other platforms (consoles, web) a malloc-based fallback is provided;
// reserve/commit/decommit are collapsed into a single allocation and the
// distinction between reserved and committed memory does not exist.

// Returns the OS memory page size in bytes.
func sz vmem_page_size(void);

// Reserves a contiguous virtual address range of at least size bytes without
// committing physical memory. Accessing the reserved pages before committing
// them is undefined behaviour (likely a hardware fault).
// Returns NULL on failure.
func void* vmem_reserve(sz size);

// Commits physical memory for the range [ptr, ptr + size).
// The range must lie entirely within a previously reserved region.
// Returns non-zero on success, zero on failure.
func b32 vmem_commit(void* ptr, sz size);

// Returns the physical memory backing [ptr, ptr + size) to the OS while keeping
// the virtual address reservation intact. The pages may be re-committed later
// with vmem_commit.
// Returns non-zero on success, zero on failure.
func b32 vmem_decommit(void* ptr, sz size);

// Releases a previously reserved region entirely, returning both the physical
// memory (if any was committed) and the virtual address space to the OS.
// ptr must be the original base address returned by vmem_reserve.
// Returns non-zero on success, zero on failure.
func b32 vmem_release(void* ptr, sz size);

// Allocates size bytes of committed virtual memory in a single call
// (equivalent to vmem_reserve + vmem_commit). Useful for large block backing
// when the reserve/commit split is not needed.
// Returns NULL on failure.
func void* vmem_alloc(sz size);

// Releases memory obtained from vmem_alloc.
// Returns non-zero on success, zero on failure.
func b32 vmem_free(void* ptr, sz size);

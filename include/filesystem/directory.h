// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef struct directory_entry {
  path full_path;
  path relative_path;
  b32 is_directory;
} directory_entry;

// Returns 1 to continue iterating, 0 to stop.
typedef b32 directory_iterate_callback(const directory_entry* entry, void* user_data);

// Creates the directory at src. Returns 1 on success, 0 otherwise.
func b32 directory_create(const path* src);

// Removes the empty directory at src. Returns 1 on success, 0 otherwise.
func b32 directory_remove(const path* src);

// Renames the directory at old_src to new_src. Returns 1 on success, 0 otherwise.
func b32 directory_rename(const path* old_src, const path* new_src);

// Copies the directory tree rooted at src into dst. Returns 1 on success, 0 otherwise.
func b32 directory_copy_recursive(const path* src, const path* dst, b32 overwrite_existing);

// Returns 1 if src exists and is a directory, 0 otherwise.
func b32 directory_exists(const path* src);

// Creates every missing directory component in src. Returns 1 on success, 0 otherwise.
func b32 directory_create_recursive(const path* src);

// Removes the directory tree rooted at src. Returns 1 on success, 0 otherwise.
func b32 directory_remove_recursive(const path* src);

// Iterates the direct children of src.
func b32 directory_iterate(const path* src, directory_iterate_callback* callback, void* user_data);

// Iterates the full directory tree rooted at src.
func b32 directory_iterate_recursive(
    const path* src,
    directory_iterate_callback* callback,
    void* user_data);

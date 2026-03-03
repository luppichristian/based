// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef struct directory_entry {
  path full_path;
  path relative_path;
  b32 is_directory;
} directory_entry;

typedef enum directory_system_path {
  DIRECTORY_SYSTEM_PATH_HOME = 0,
  DIRECTORY_SYSTEM_PATH_DESKTOP = 1,
  DIRECTORY_SYSTEM_PATH_DOCUMENTS = 2,
  DIRECTORY_SYSTEM_PATH_DOWNLOADS = 3,
  DIRECTORY_SYSTEM_PATH_MUSIC = 4,
  DIRECTORY_SYSTEM_PATH_PICTURES = 5,
  DIRECTORY_SYSTEM_PATH_PUBLICSHARE = 6,
  DIRECTORY_SYSTEM_PATH_SAVEDGAMES = 7,
  DIRECTORY_SYSTEM_PATH_SCREENSHOTS = 8,
  DIRECTORY_SYSTEM_PATH_TEMPLATES = 9,
  DIRECTORY_SYSTEM_PATH_VIDEOS = 10,
  DIRECTORY_SYSTEM_PATH_COUNT = 11,
} directory_system_path;

// Returns 1 to continue iterating, 0 to stop.
typedef b32 directory_iterate_callback(const directory_entry* entry, void* user_data);

// Returns the application base directory, or an empty path on failure.
func path directory_get_base(void);

// Returns the per-user application data directory for org_name/app_name, or an empty path on failure.
func path directory_get_pref(const c8* org_name, const c8* app_name);

// Returns the requested user-facing system directory, or an empty path when unavailable.
func path directory_get_system(directory_system_path location);

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

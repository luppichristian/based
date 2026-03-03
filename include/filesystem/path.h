// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../strings/strings.h"
#include "../utils/timestamp.h"

#define PATH_CAP 512

typedef struct path {
  c8 buf[PATH_CAP];
} path;

static_assert(size_of(path) == PATH_CAP);

// Copies src into a new path value.
func path path_from_cstr(const c8* src);

// Copies src into a new path value.
func path path_from_str8(str8 src);

// Joins lhs and rhs using a single '/' separator when needed.
// If rhs is absolute, rhs replaces lhs.
func path path_join(const path* lhs, const path* rhs);

// Joins lhs with a c-string rhs using a single '/' separator when needed.
func path path_join_cstr(const path* lhs, const c8* rhs);

// Appends src to dst as a path segment.
func sz path_append(path* dst, const path* src);

// Appends a c-string path segment to dst.
func sz path_append_cstr(path* dst, const c8* src);

// Normalizes separators to '/' and collapses duplicate separators.
func void path_normalize(path* src);

// Returns 1 if src ends with suffix, 0 otherwise.
func b32 path_ends_with(const path* src, const c8* suffix);

// Returns 1 if src is absolute on the current platform style, 0 otherwise.
func b32 path_is_absolute(const path* src);

// Removes the trailing file extension, if any.
func void path_remove_extension(path* src);

// Removes the final path component, preserving the root when present.
func void path_remove_name(path* src);

// Removes the directory portion, leaving only the final path component.
func void path_remove_directory(path* src);

// Removes trailing separators while preserving the root path.
func void path_remove_trailing_slash(path* src);

// Returns the file extension (including '.') or an empty path if none exists.
func path path_get_extension(const path* src);

// Returns the final path component.
func path path_get_name(const path* src);

// Returns the final path component without its extension.
func path path_get_basename(const path* src);

// Returns the directory portion.
func path path_get_directory(const path* src);

// Returns the common directory prefix shared by src_list.
func path path_get_common(const path* src_list, sz path_count);

// Returns 1 if a filesystem entry exists at src, 0 otherwise.
func b32 path_exists(const path* src);

// Removes the filesystem entry at src. Returns 1 on success, 0 otherwise.
func b32 path_remove(const path* src);

// Renames old_src to new_src. Returns 1 on success, 0 otherwise.
func b32 path_rename(const path* old_src, const path* new_src);

// Returns the last write time as Unix-epoch microseconds, or zero on failure.
func timestamp path_get_last_write_time(const path* src);

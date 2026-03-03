// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef enum filewatch_action {
  FILEWATCH_ACTION_CREATE = 1,
  FILEWATCH_ACTION_DELETE = 2,
  FILEWATCH_ACTION_MODIFY = 3,
  FILEWATCH_ACTION_MOVE = 4,
} filewatch_action;

typedef struct filewatch_event {
  i64 watch_id;
  filewatch_action action;
  path watch_path;
  path item_path;
  path old_item_path;
} filewatch_event;

typedef void filewatch_event_callback(const filewatch_event* event, void* user_data);
typedef void filewatch_missed_callback(const path* watch_path, void* user_data);

typedef struct filewatch {
  void* native_handle;
  filewatch_event_callback* event_fn;
  filewatch_missed_callback* missed_fn;
  void* user_data;
} filewatch;

// Creates a filewatch wrapper around efsw.
func filewatch filewatch_create(
    filewatch_event_callback* event_fn,
    filewatch_missed_callback* missed_fn,
    void* user_data,
    b32 use_generic_mode);

// Releases the watcher and every active watch.
func void filewatch_destroy(filewatch* watcher);

// Starts the background watch thread. Returns 1 on success, 0 otherwise.
func b32 filewatch_start(filewatch* watcher);

// Adds a watched directory. Returns a positive watch id on success, or a negative error code.
func i64 filewatch_add(filewatch* watcher, const path* src, b32 recursive);

// Removes the watch identified by watch_id. Returns 1 on success, 0 otherwise.
func b32 filewatch_remove(filewatch* watcher, i64 watch_id);

// Removes the watch rooted at src. Returns 1 on success, 0 otherwise.
func b32 filewatch_remove_path(filewatch* watcher, const path* src);

// Mirrors efsw follow-symlink behaviour. Returns 1 on success, 0 otherwise.
func b32 filewatch_follow_symlinks(filewatch* watcher, b32 enabled);

// Mirrors efsw out-of-scope link behaviour. Returns 1 on success, 0 otherwise.
func b32 filewatch_allow_out_of_scope_links(filewatch* watcher, b32 enabled);

// Returns the last efsw error string.
func const c8* filewatch_get_last_error(void);

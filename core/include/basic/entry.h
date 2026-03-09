// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../memory/vmem.h"
#include "../utils/cmdline.h"
#include "env_defines.h"
#include "primitive_types.h"

// =========================================================================
// Shared Program Lifecycle
// =========================================================================

// Common initialization hook used by custom entry points.
// The built-in entry paths above also route through this function internally.
// Returns true on success, false on failure.
func b32 entry_init(cmdline cmdline);

// Returns the command line captured during the first successful entry_init().
func cmdline entry_get_cmdline(void);

// Common shutdown hook paired with entry_init().
// Safe to call even if initialization failed.
func void entry_quit(void);

// Enum to describe app result
typedef enum app_result {
  APP_RESULT_CONTINUE,  // Continue calling update().
  APP_RESULT_SUCCESS,   // Exit successfully.
  APP_RESULT_FAIL,      // Exit with failure.
  APP_RESULT_MAX,
} app_result;

typedef app_result entry_app_init_fn(cmdline cmdl, void** state);
typedef app_result entry_app_update_fn(void* state);
typedef void entry_app_quit_fn(void* state);
typedef b32 entry_run_fn(cmdline cmdl);

func void entry_set_app_callbacks(
    entry_app_init_fn* init_fn,
    entry_app_update_fn* update_fn,
    entry_app_quit_fn* quit_fn);

func void entry_set_run_callback(entry_run_fn* run_fn);

// =========================================================================
// Application Entry Points
// =========================================================================

#if defined(ENTRY_TYPE_APP)

// SDL-style application lifecycle used for cross-platform entry handling.
// The runtime owns the outer platform entry point and forwards the parsed
// command line plus caller-managed state through this interface.

// Called once before the main loop starts.
// Implementations can allocate and store user state through state.
func app_result app_init(cmdline cmdl, void** state);

// Called repeatedly until it returns APP_RESULT_SUCCESS or APP_RESULT_FAIL.
func app_result app_update(void* state);

// Called once during shutdown, even after a failed init().
func void app_quit(void* state);

#  define ENTRY_FUNCTION_NAME        main_app
#  define ENTRY_REGISTER_CALLBACKS() entry_set_app_callbacks(app_init, app_update, app_quit)

#elif defined(ENTRY_TYPE_RUN)

// Simple command-line entry point wrapper.
// Returns true on success, false on failure.
func b32 run(cmdline cmdl);

#  define ENTRY_FUNCTION_NAME        main_run
#  define ENTRY_REGISTER_CALLBACKS() entry_set_run_callback(run)

#elif defined(ENTRY_TYPE_MOD)

// Module lifecycle used by the custom dynamic-module loader.
// mod_init() runs when the module is loaded and mod_quit() runs when it
// is unloaded.

// Exported wrapper used by the module loader.
func dll_export b32 mod_init(void);

// Exported wrapper used by the module loader.
func dll_export void mod_quit(void);

#endif

#ifndef ENTRY_REGISTER_CALLBACKS
#  define ENTRY_REGISTER_CALLBACKS() ((void)0)
#endif
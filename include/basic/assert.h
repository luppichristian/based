// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "codespace.h"
#include "primitive_types.h"

// =========================================================================
// Assert function
// =========================================================================

// If assert is already defined, undefine it to avoid conflicts with our own implementation.
#ifdef assert
#  undef assert
#endif

// Assert function called by the ASSERT macro.
// It will act depending on the internal setup.
func void _assert(b32 condition, const char* msg, callsite site);

// Convenience macro for assertions. It will call the _assert function.
#define assert(condition) _assert((condition), #condition, CALLSITE_HERE)

// =========================================================================
// Assert mode
// =========================================================================

// Different modes settable at runtime.
typedef enum assert_mode {
  ASSERT_MODE_DEBUG,   // On Desktop: Opens a debug window and waits for user input, in
                       // the window you can choose to break point, ignore or quit the application.
                       // The message is also logged.
  ASSERT_MODE_QUIT,    // Quits the application immediately.
  ASSERT_MODE_LOG,     // Just log a message.
  ASSERT_MODE_IGNORE,  // Ignores the assertion, does nothing.
} assert_mode;

// Define default assert mode if not defined by the user.
#ifndef ASSERT_MODE_DEFAULT
#  ifdef BUILD_DEBUG
#    define ASSERT_MODE_DEFAULT ASSERT_MODE_DEBUG
#  else
#    define ASSERT_MODE_DEFAULT ASSERT_MODE_LOG
#  endif
#endif

// Set the desired assert mode at runtime. By default, it is set to ASSERT_MODE_DEFAULT.
func void assert_set_mode(assert_mode mode);

// =========================================================================
// Assert callback
// =========================================================================

// Function prototype for the assert callback.
// This callback can be set by the user to handle assertions in a custom way.
// If 'true' is returned, the default assert handling will be called,
// otherwise it will be ignored.
typedef b32 (*assert_callback)(const char* msg, callsite site);

// Set the assert callback function. This allows the user to handle assertions in a custom way.
func void assert_set_callback(assert_callback callback);
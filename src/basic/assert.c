// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/assert.h"
#include "basic/log.h"
#include "../sdl3_include.h"

#include <stdlib.h>

// =========================================================================
// Internal state
// =========================================================================

global_var assert_mode assert_mode_current = ASSERT_MODE_DEFAULT;
global_var assert_callback assert_callback_current = NULL;

// =========================================================================
// Internal helpers
// =========================================================================

func void assert_log_msg(const c8* msg, callsite site) {
  _log(LOG_LEVEL_FATAL, site, "Assertion failed: %s", msg);
}

// Returns: 0 = ignore, 1 = breakpoint, 2 = quit.
// Defaults to quit if the message box cannot be displayed.
func i32 assert_dialog(const c8* msg, callsite site) {
  // TODO: Only do this if on desktop...
  // TODO: Proper message.
  c8 buf[1024];
  SDL_snprintf(buf, sizeof(buf), "Assertion failed: %s\n\nin %s() at %s:%u", msg, site.function, site.filename, site.line);

  SDL_MessageBoxButtonData buttons[] = {
      {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Breakpoint"},
      {                                      0, 0,     "Ignore"},
      {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2,       "Quit"},
  };

  SDL_MessageBoxData data = {
      SDL_MESSAGEBOX_ERROR,
      NULL,
      "Assertion Failed",
      buf,
      SDL_arraysize(buttons),
      buttons,
      NULL,
  };

  int btn_id = 2;
  SDL_ShowMessageBox(&data, &btn_id);
  return (i32)btn_id;
}

// =========================================================================
// Public API
// =========================================================================

func void assert_set_mode(assert_mode mode) {
  assert_mode_current = mode;
}

func void assert_set_callback(assert_callback callback) {
  assert_callback_current = callback;
}

func void _assert(b32 condition, const c8* msg, callsite site) {
  if (condition) {
    return;
  }

  if (assert_callback_current != NULL) {
    b32 should_continue = assert_callback_current(msg, site);
    if (!should_continue) {
      return;
    }
  }

  switch (assert_mode_current) {
    case ASSERT_MODE_DEBUG: {
      assert_log_msg(msg, site);
      i32 action = assert_dialog(msg, site);
      if (action == 1) {
        SDL_TriggerBreakpoint();
      } else if (action == 2) {
        exit(1);
      }
      break;
    }
    case ASSERT_MODE_QUIT:
      assert_log_msg(msg, site);
      exit(1);
    case ASSERT_MODE_LOG:
      assert_log_msg(msg, site);
      break;
    case ASSERT_MODE_IGNORE:
      break;
  }
}

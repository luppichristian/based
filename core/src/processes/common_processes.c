// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/common_processes.h"
#include "basic/env_defines.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"
#include "basic/utility_defines.h"
#include "processes/process.h"
#include "../sdl3_include.h"
#include "strings/cstrings.h"
#include <stdio.h>

func b32 common_processes_spawn_background(cstr8 const* args, cstr8 cwd_path) {
  profile_func_begin;
  if (args == NULL || args[0] == NULL) {
    thread_log_error("Rejected background process spawn without arguments");
    profile_func_end;
    return false;
  }

  process_options options = process_options_default();
  options.background = true;
  options.cwd = cwd_path;
  process prc = process_create_with(args, options);
  if (!process_is_valid(prc)) {
    thread_log_error("Failed to spawn background process command=%s cwd=%s",
                     args[0],
                     cwd_path != NULL ? cwd_path : "<inherit>");
    profile_func_end;
    return false;
  }

  thread_log_info("Spawned background process command=%s cwd=%s",
                  args[0],
                  cwd_path != NULL ? cwd_path : "<inherit>");
  process_destroy(prc);
  profile_func_end;
  return true;
}

func b32 process_open_weblink(cstr8 url) {
  profile_func_begin;
  if (url == NULL || url[0] == '\0') {
    thread_log_error("Rejected web link open for empty URL");
    profile_func_end;
    return false;
  }

  b32 result = SDL_OpenURL(url);
  if (!result) {
    thread_log_error("Failed to open web link url=%s error=%s", url, SDL_GetError());
  } else {
    thread_log_info("Opened web link url=%s", url);
  }
  profile_func_end;
  return result;
}

func b32 process_open_file_window(cstr8 location) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS)
  cstr8 const args[] = {
      "explorer.exe",
      location != NULL ? location : ".",
      NULL,
  };
#elif defined(PLATFORM_MACOS)
  cstr8 const args[] = {
      "open",
      location != NULL ? location : ".",
      NULL,
  };
#elif defined(PLATFORM_LINUX)
  cstr8 const args[] = {
      "xdg-open",
      location != NULL ? location : ".",
      NULL,
  };
#else
  invalid_code_path;
  profile_func_end;
  return false;
#endif
  b32 result = common_processes_spawn_background(args, NULL);
  if (!result) {
    thread_log_error("Failed to open file window location=%s", location != NULL ? location : ".");
  }
  profile_func_end;
  return result;
}

func b32 process_open_terminal(cstr8 location) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS)
  c8 cmd_text[1024] = {0};
  cstr8 dir_path = location != NULL ? location : ".";
  (void)cstr8_format(cmd_text, size_of(cmd_text), "cd /d \"%s\"", dir_path);

  cstr8 const args[] = {
      "cmd.exe",
      "/K",
      cmd_text,
      NULL,
  };
  b32 result = common_processes_spawn_background(args, NULL);
  if (!result) {
    thread_log_error("Failed to open terminal location=%s", dir_path);
  }
  profile_func_end;
  return result;
#elif defined(PLATFORM_MACOS)
  cstr8 const args[] = {
      "open",
      "-a",
      "Terminal",
      location != NULL ? location : ".",
      NULL,
  };
  b32 result = common_processes_spawn_background(args, NULL);
  if (!result) {
    thread_log_error("Failed to open terminal location=%s", location != NULL ? location : ".");
  }
  profile_func_end;
  return result;
#elif defined(PLATFORM_LINUX)
  cstr8 const xterm_args[] = {
      "x-terminal-emulator",
      NULL,
  };
  if (common_processes_spawn_background(xterm_args, location)) {
    profile_func_end;
    return true;
  }

  cstr8 const gnome_args[] = {
      "gnome-terminal",
      "--working-directory",
      location != NULL ? location : ".",
      NULL,
  };
  if (common_processes_spawn_background(gnome_args, NULL)) {
    profile_func_end;
    return true;
  }

  cstr8 const konsole_args[] = {
      "konsole",
      "--workdir",
      location != NULL ? location : ".",
      NULL,
  };
  if (common_processes_spawn_background(konsole_args, NULL)) {
    profile_func_end;
    return true;
  }

  cstr8 const xterm_plain_args[] = {
      "xterm",
      NULL,
  };
  b32 result = common_processes_spawn_background(xterm_plain_args, location);
  if (!result) {
    thread_log_error("Failed to open terminal location=%s", location != NULL ? location : ".");
  }
  profile_func_end;
  return result;
#else
  invalid_code_path;
  profile_func_end;
  return false;
#endif
}

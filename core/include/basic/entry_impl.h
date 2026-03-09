// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "entry.h"

// Internal implementation guard for this header-only wrapper block.
// This should not be defined manually.

// Define ENTRY_HANDLED if you want to include this header without generating
// any entry-point wrapper implementation.

// Define ENTRY_NOIMPL when the wrapper implementation is generated in another
// translation unit.

// You can define ENTRY_GET_GLOBAL_ALLOCATOR if you whish to override the global allocator,
// that's passed in internal functions.
#ifndef ENTRY_GET_GLOBAL_ALLOCATOR
#  define ENTRY_GET_GLOBAL_ALLOCATOR vmem_get_allocator
#endif

// You can define ENTRY_WINDOWS_CONSOLE if you whish to not use the windows subsystem.
// If it's not defined, WinMain will be used instead of regular main.

// Implement the actual entry point.
#if defined(ENTRY_FUNCTION_NAME) && !defined(ENTRY_HANDLED) && !defined(ENTRY_NOIMPL)

#  ifndef ENTRY_IMPL
#    define ENTRY_IMPL

// This is the entry point we are going to call internally.
#    ifdef __cplusplus
extern "C" int ENTRY_FUNCTION_NAME(int argc, char** argv, allocator alloc);
#    else
int ENTRY_FUNCTION_NAME(int argc, char** argv, allocator alloc);
#    endif

#    if defined(PLATFORM_WINDOWS) && !defined(ENTRY_WINDOWS_CONSOLE)

#      ifndef WINAPI
#        define WINAPI __stdcall
#      endif

typedef struct HINSTANCE__* HINSTANCE;
typedef char* LPSTR;
typedef wchar_t* PWSTR;

#      if defined(_MSC_VER)
#        if defined(UNICODE) && UNICODE
int wmain(int argc, wchar_t** wargv, wchar_t** wenvp) {
  (void)argc;
  (void)wargv;
  (void)wenvp;
  ENTRY_REGISTER_CALLBACKS();
  return ENTRY_FUNCTION_NAME(0, NULL, ENTRY_GET_GLOBAL_ALLOCATOR());
}

#        else
int main(int argc, char** argv) {
  ENTRY_REGISTER_CALLBACKS();
  return ENTRY_FUNCTION_NAME(argc, argv, ENTRY_GET_GLOBAL_ALLOCATOR());
}

#        endif
#      endif

#      ifdef __cplusplus
extern "C" {
#      endif

#      if defined(UNICODE) && UNICODE
int WINAPI wWinMain(HINSTANCE hinst, HINSTANCE hprev, PWSTR cmdline, int show_cmd) {
#      else
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR cmdline, int show_cmd) {
#      endif
  (void)hinst;
  (void)hprev;
  (void)cmdline;
  (void)show_cmd;
  ENTRY_REGISTER_CALLBACKS();
  return ENTRY_FUNCTION_NAME(0, NULL, ENTRY_GET_GLOBAL_ALLOCATOR());
}

#      ifdef __cplusplus
}
#      endif

#    else

int main(int argc, char** argv) {
  ENTRY_REGISTER_CALLBACKS();
  return ENTRY_FUNCTION_NAME(argc, argv, ENTRY_GET_GLOBAL_ALLOCATOR());
}

#    endif
#  endif
#endif

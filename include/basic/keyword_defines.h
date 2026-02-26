// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Required for compiler detection and platform-specific definitions.
#include "env_defines.h"

/*
Define custom keywords used in the project.
I like custom keywords, they make the code more readable to me, you can 100% tell
what they do even if they are not standard C keywords.
Some compilers/platforms have different names for the same thing,
so I want to have a single place where I can define them and use them throughout the project.

Functional keywords:
- 'thread_local' for thread-local storage.
- 'dllexport' for exporting symbols from a shared library.
- 'dllimport' for importing symbols from a shared library.
- 'static_assert' for compile-time assertions.
- 'unused' for marking variables or functions as intentionally unused to avoid compiler warnings.
- 'deprecated' for marking functions or variables as deprecated.
- 'noreturn' for functions that do not return to the caller.
- 'forceinline' for suggesting the compiler to inline a function.
- 'noinline' for suggesting the compiler not to inline a function.
- 'alignas' for specifying the alignment of a variable or type.
- 'alignof' for querying the alignment requirement of a type.
- 'likely' and 'unlikely' for branch prediction hints to the compiler.
- 'readonly' for variables that are constant and should not be modified after init.

Purely cosmetic keywords:
- 'func' as a shorthand for 'function' to improve readability.
- 'global_var' for global variables to make it clear they are global.
- 'local_persist' for static variables that should persist across function calls.

If ALL_FUNCS_STATIC is defined, all functions will be declared as static.
If ALL_GLOBAL_VARS_STATIC is defined, all global variables will be declared as static.
*/

// =========================================================================
// Functional Keywords
// =========================================================================

// thread_local — thread-local storage duration.
#if defined(__cplusplus) && __cplusplus >= 201103L
// C++11 keyword — already defined.
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define thread_local _Thread_local
#elif defined(COMPILER_MSVC)
#  define thread_local __declspec(thread)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define thread_local __thread
#else
#  define thread_local
#endif

// dllexport / dllimport — symbol visibility for shared libraries.
#if defined(PLATFORM_WINDOWS)
#  define dllexport __declspec(dllexport)
#  define dllimport __declspec(dllimport)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define dllexport __attribute__((visibility("default")))
#  define dllimport
#else
#  define dllexport
#  define dllimport
#endif

// static_assert — compile-time assertion.
// Guard against redefining a language keyword or existing macro.
#ifndef static_assert
#  if defined(__cplusplus) && __cplusplus >= 201103L
// C++11 keyword — already defined.
#  elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define static_assert _Static_assert
#  else
#    define static_assert(expr, msg) typedef char static_assert_##__COUNTER__[(expr) ? 1 : -1]
#  endif
#endif

// unused — suppress unused variable/parameter warnings.
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define unused [[maybe_unused]]
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define unused __attribute__((unused))
#else
#  define unused
#endif

// deprecated — mark a symbol as deprecated.
#if defined(__cplusplus) && __cplusplus >= 201402L
#  define deprecated [[deprecated]]
#elif defined(COMPILER_MSVC)
#  define deprecated __declspec(deprecated)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define deprecated __attribute__((deprecated))
#else
#  define deprecated
#endif

// noreturn — function does not return to the caller.
#ifndef noreturn
#  if defined(__cplusplus) && __cplusplus >= 201103L
#    define noreturn [[noreturn]]
#  elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define noreturn _Noreturn
#  elif defined(COMPILER_MSVC)
#    define noreturn __declspec(noreturn)
#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
      defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#    define noreturn __attribute__((noreturn))
#  else
#    define noreturn
#  endif
#endif

// forceinline — ask the compiler to always inline this function.
#if defined(COMPILER_MSVC) || defined(COMPILER_INTEL)
#  define forceinline __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG)
#  define forceinline __attribute__((always_inline)) inline
#else
#  define forceinline inline
#endif

// noinline — ask the compiler never to inline this function.
#if defined(COMPILER_MSVC) || defined(COMPILER_INTEL)
#  define noinline __declspec(noinline)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG)
#  define noinline __attribute__((noinline))
#else
#  define noinline
#endif

// alignas — specify the alignment of a variable or type.
#ifndef alignas
#  if defined(__cplusplus) && __cplusplus >= 201103L
// C++11 keyword — already defined.
#  elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define alignas(x) _Alignas(x)
#  elif defined(COMPILER_MSVC)
#    define alignas(x) __declspec(align(x))
#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
      defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#    define alignas(x) __attribute__((aligned(x)))
#  else
#    define alignas(x)
#  endif
#endif

// alignof — query the alignment requirement of a type.
#ifndef alignof
#  if defined(__cplusplus) && __cplusplus >= 201103L
// C++11 keyword — already defined.
#  elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define alignof(x) _Alignof(x)
#  elif defined(COMPILER_MSVC)
#    define alignof(x) __alignof(x)
#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
      defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#    define alignof(x) __alignof__(x)
#  else
#    define alignof(x) sizeof(x)
#  endif
#endif

// likely / unlikely — branch prediction hints.
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

// readonly — constant variable explicitly placed in the read-only data section.
// MSVC  → .rdata   (PE read-only data segment)
// Linux → .rodata  (ELF read-only data segment)
// macOS → __TEXT,__const (Mach-O read-only text segment)
// Note: must have a compile-time constant initializer; use at global/file scope only.
#if defined(COMPILER_MSVC)
#  pragma section(".rdata", read)
#  if defined(ALL_GLOBAL_VARS_STATIC)
#    define readonly static __declspec(allocate(".rdata")) const
#  else
#    define readonly __declspec(allocate(".rdata")) const
#  endif
#elif defined(COMPILER_APPLE_CLANG)
#  if defined(ALL_GLOBAL_VARS_STATIC)
#    define readonly static const __attribute__((section("__TEXT,__const")))
#  else
#    define readonly const __attribute__((section("__TEXT,__const")))
#  endif
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_INTEL)
#  if defined(ALL_GLOBAL_VARS_STATIC)
#    define readonly static const __attribute__((section(".rodata")))
#  else
#    define readonly const __attribute__((section(".rodata")))
#  endif
#else
#  if defined(ALL_GLOBAL_VARS_STATIC)
#    define readonly static const
#  else
#    define readonly const
#  endif
#endif

// =========================================================================
// Cosmetic Keywords
// =========================================================================

// local_persist — static variable that retains its value across calls.
#define local_persist static

// global_var — global variable (purely cosmetic, improves readability).
#if defined(ALL_GLOBAL_VARS_STATIC)
#  define global_var static
#else
#  define global_var
#endif

// func — function declaration; static if ALL_FUNCS_STATIC is defined.
#if defined(ALL_FUNCS_STATIC)
#  define func static
#else
#  define func
#endif
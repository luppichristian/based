// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/id.h"
#include "basic/assert.h"
#include "basic/profiler.h"

#include <errno.h>
#include <stdlib.h>

// Shared decimal digit counter reused by all ID widths.
func sz id_u64_digits(u64 value) {
  profile_func_begin;
  sz digits = 1;
  while (value >= 10ULL) {
    value /= 10ULL;
    digits++;
  }
  profile_func_end;
  return digits;
}

// Shared unsigned decimal parser with a caller-supplied upper bound.
func b32 id_parse_u64(cstr8 src, u64 max_value, u64* out) {
  profile_func_begin;
  if (src == NULL || out == NULL) {
    profile_func_end;
    return 0;
  }
  assert(src != NULL);
  assert(out != NULL);
  if (src[0] == '\0') {
    profile_func_end;
    return 0;
  }

  char* end_ptr;
  errno = 0;
  unsigned long long parsed = strtoull(src, &end_ptr, 10);
  if (errno != 0 || end_ptr == src || *end_ptr != '\0') {
    profile_func_end;
    return 0;
  }
  if ((u64)parsed > max_value) {
    profile_func_end;
    return 0;
  }

  *out = (u64)parsed;
  profile_func_end;
  return 1;
}

// Generates the same helper set for each fixed-width ID wrapper.
// Only the underlying storage type and parse limit differ between variants.
#define ID_DEFINE_TYPE(NAME, TYPE, MAX_VALUE)                               \
  func NAME NAME##_zero(void) {                                             \
    profile_func_begin;                                                     \
    NAME ident = {.value = 0};                                              \
    profile_func_end;                                                       \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func NAME NAME##_make(TYPE value) {                                       \
    profile_func_begin;                                                     \
    NAME ident = {.value = value};                                          \
    profile_func_end;                                                       \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func TYPE NAME##_get(NAME ident) {                                        \
    profile_func_begin;                                                     \
    profile_func_end;                                                       \
    return ident.value;                                                     \
  }                                                                         \
                                                                            \
  func b32 NAME##_is_zero(NAME ident) {                                     \
    profile_func_begin;                                                     \
    profile_func_end;                                                       \
    return ident.value == (TYPE)0 ? 1 : 0;                                  \
  }                                                                         \
                                                                            \
  func b32 NAME##_is_valid(NAME ident) {                                    \
    profile_func_begin;                                                     \
    profile_func_end;                                                       \
    return ident.value != (TYPE)0 ? 1 : 0;                                  \
  }                                                                         \
                                                                            \
  func NAME NAME##_next(NAME ident) {                                       \
    profile_func_begin;                                                     \
    ident.value = (TYPE)(ident.value + 1);                                  \
    profile_func_end;                                                       \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func i32 NAME##_cmp(NAME lhs, NAME rhs) {                                 \
    profile_func_begin;                                                     \
    if (lhs.value < rhs.value) {                                            \
      profile_func_end;                                                     \
      return -1;                                                            \
    }                                                                       \
    if (lhs.value > rhs.value) {                                            \
      profile_func_end;                                                     \
      return 1;                                                             \
    }                                                                       \
    profile_func_end;                                                       \
    return 0;                                                               \
  }                                                                         \
                                                                            \
  func sz NAME##_string_length(NAME ident) {                                \
    profile_func_begin;                                                     \
    profile_func_end;                                                       \
    return id_u64_digits((u64)ident.value);                                 \
  }                                                                         \
                                                                            \
  func b32 NAME##_parse_cstr8(cstr8 src, NAME* out) {                       \
    profile_func_begin;                                                     \
    u64 parsed = 0;                                                         \
    if (!id_parse_u64(src, (u64)(MAX_VALUE), &parsed)) {                    \
      profile_func_end;                                                     \
      return 0;                                                             \
    }                                                                       \
    *out = NAME##_make((TYPE)parsed);                                       \
    profile_func_end;                                                       \
    return 1;                                                               \
  }                                                                         \
                                                                            \
  func b32 NAME##_to_cstr8(NAME ident, c8* dst, sz cap) {                   \
    profile_func_begin;                                                     \
    sz needed = NAME##_string_length(ident) + 1;                            \
    if (cap < needed) {                                                     \
      if (cap > 0) {                                                        \
        dst[0] = '\0';                                                      \
      }                                                                     \
      profile_func_end;                                                     \
      return 0;                                                             \
    }                                                                       \
    profile_func_end;                                                       \
    return cstr8_format(dst, cap, "%llu", (unsigned long long)ident.value); \
  }                                                                         \
                                                                            \
  func b32 NAME##_to_str8(NAME ident, str8* dst) {                          \
    profile_func_begin;                                                     \
    if (dst->cap == 0) {                                                    \
      dst->size = 0;                                                        \
      profile_func_end;                                                     \
      return 0;                                                             \
    }                                                                       \
    b32 success = NAME##_to_cstr8(ident, dst->ptr, dst->cap);               \
    dst->size = cstr8_len(dst->ptr);                                        \
    profile_func_end;                                                       \
    return success;                                                         \
  }

// Instantiate the shared helpers for each supported ID width.
ID_DEFINE_TYPE(id8, u8, U8_MAX)
ID_DEFINE_TYPE(id16, u16, U16_MAX)
ID_DEFINE_TYPE(id32, u32, U32_MAX)
ID_DEFINE_TYPE(id64, u64, U64_MAX)

#undef ID_DEFINE_TYPE

// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "keyword_defines.h"

#include <float.h>
#include <stddef.h>
#include <stdint.h>

/*
Primitive types:
- `i8`: 8-bit signed integer
- `u8`: 8-bit unsigned integer
- `i16`: 16-bit signed integer
- `u16`: 16-bit unsigned integer
- `i32`: 32-bit signed integer
- `u32`: 32-bit unsigned integer
- `i64`: 64-bit signed integer
- `u64`: 64-bit unsigned integer
- `f32`: 32-bit floating-point number
- `f64`: 64-bit floating-point number
- `b8`: 8-bit boolean
- `b16`: 16-bit boolean
- `b32`: 32-bit boolean
- `b64`: 64-bit boolean
- `c8`: 8-bit character
- `c16`: 16-bit character
- `c32`: 32-bit character
- `i8x`: 8-bit fast signed integer (at least 8 bits)
- `u8x`: 8-bit fast unsigned integer (at least 8 bits)
- `i16x`: 16-bit fast signed integer (at least 16 bits)
- `u16x`: 16-bit fast unsigned integer (at least 16 bits)
- `i32x`: 32-bit fast signed integer (at least 32 bits)
- `u32x`: 32-bit fast unsigned integer (at least 32 bits)
- `i64x`: 64-bit fast signed integer (at least 64 bits)
- `u64x`: 64-bit fast unsigned integer (at least 64 bits)
- `sz`: platform-width unsigned size type (`size_t`)
- `up`: platform-width unsigned pointer integer (`uintptr_t`)
- `sp`: platform-width signed pointer integer (`intptr_t`)
- `dp`: platform-width signed pointer difference (`ptrdiff_t`)

For each type the following constants are also defined:
- `TYPE_MIN`: Minimum value of the type (e.g., `I8_MIN`, `U8_MIN`, etc.)
- `TYPE_MAX`: Maximum value of the type (e.g., `I8_MAX`, `U8_MAX`, etc.)
*/

// =========================================================================
// Exact-width integer types
// =========================================================================

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

read_only global_var i8 I8_MIN = -128;
read_only global_var i8 I8_MAX = 127;
read_only global_var u8 U8_MIN = 0;
read_only global_var u8 U8_MAX = 255;
read_only global_var i16 I16_MIN = -32768;
read_only global_var i16 I16_MAX = 32767;
read_only global_var u16 U16_MIN = 0;
read_only global_var u16 U16_MAX = 65535;
read_only global_var i32 I32_MIN = -2147483647 - 1;
read_only global_var i32 I32_MAX = 2147483647;
read_only global_var u32 U32_MIN = 0;
read_only global_var u32 U32_MAX = 4294967295U;
read_only global_var i64 I64_MIN = -9223372036854775807LL - 1;
read_only global_var i64 I64_MAX = 9223372036854775807LL;
read_only global_var u64 U64_MIN = 0;
read_only global_var u64 U64_MAX = 18446744073709551615ULL;

// =========================================================================
// Floating-point types
// =========================================================================

typedef float f32;
typedef double f64;

read_only global_var f32 F32_MIN = -FLT_MAX;
read_only global_var f32 F32_MAX = FLT_MAX;
read_only global_var f64 F64_MIN = -DBL_MAX;
read_only global_var f64 F64_MAX = DBL_MAX;

// =========================================================================
// Boolean types
// =========================================================================

typedef uint8_t b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef uint64_t b64;

read_only global_var b8 B8_MIN = 0;
read_only global_var b8 B8_MAX = 1;
read_only global_var b16 B16_MIN = 0;
read_only global_var b16 B16_MAX = 1;
read_only global_var b32 B32_MIN = 0;
read_only global_var b32 B32_MAX = 1;
read_only global_var b64 B64_MIN = 0;
read_only global_var b64 B64_MAX = 1;

// =========================================================================
// Character types
// =========================================================================

typedef char c8;
typedef uint16_t c16;
typedef uint32_t c32;

read_only global_var c8 C8_MIN = 0;
read_only global_var c8 C8_MAX = (c8)255;
read_only global_var c16 C16_MIN = 0;
read_only global_var c16 C16_MAX = 65535;
read_only global_var c32 C32_MIN = 0;
read_only global_var c32 C32_MAX = 4294967295U;

// =========================================================================
// Fast integer types (at least N bits, platform's fastest representation)
// =========================================================================

typedef int_fast8_t i8x;
typedef uint_fast8_t u8x;
typedef int_fast16_t i16x;
typedef uint_fast16_t u16x;
typedef int_fast32_t i32x;
typedef uint_fast32_t u32x;
typedef int_fast64_t i64x;
typedef uint_fast64_t u64x;

read_only global_var i8x I8X_MIN = -128;
read_only global_var i8x I8X_MAX = 127;
read_only global_var u8x U8X_MIN = 0;
read_only global_var u8x U8X_MAX = 255;
read_only global_var i16x I16X_MIN = -32768;
read_only global_var i16x I16X_MAX = 32767;
read_only global_var u16x U16X_MIN = 0;
read_only global_var u16x U16X_MAX = 65535;
read_only global_var i32x I32X_MIN = -2147483647 - 1;
read_only global_var i32x I32X_MAX = 2147483647;
read_only global_var u32x U32X_MIN = 0;
read_only global_var u32x U32X_MAX = 4294967295U;
read_only global_var i64x I64X_MIN = -9223372036854775807LL - 1;
read_only global_var i64x I64X_MAX = 9223372036854775807LL;
read_only global_var u64x U64X_MIN = 0;
read_only global_var u64x U64X_MAX = 18446744073709551615ULL;

// =========================================================================
// Pointer and size types
// =========================================================================

typedef size_t    sz;
typedef uintptr_t up;
typedef intptr_t  sp;
typedef ptrdiff_t dp;

read_only global_var sz SZ_MIN = 0;
read_only global_var sz SZ_MAX = SIZE_MAX;
read_only global_var up UP_MIN = 0;
read_only global_var up UP_MAX = UINTPTR_MAX;
read_only global_var sp SP_MIN = INTPTR_MIN;
read_only global_var sp SP_MAX = INTPTR_MAX;
read_only global_var dp DP_MIN = PTRDIFF_MIN;
read_only global_var dp DP_MAX = PTRDIFF_MAX;

// =========================================================================
// Compile-time size guarantees
// =========================================================================

static_assert(sizeof(i8) == 1, "primitive_types.h: i8 must be 1 byte");
static_assert(sizeof(u8) == 1, "primitive_types.h: u8 must be 1 byte");
static_assert(sizeof(i16) == 2, "primitive_types.h: i16 must be 2 bytes");
static_assert(sizeof(u16) == 2, "primitive_types.h: u16 must be 2 bytes");
static_assert(sizeof(i32) == 4, "primitive_types.h: i32 must be 4 bytes");
static_assert(sizeof(u32) == 4, "primitive_types.h: u32 must be 4 bytes");
static_assert(sizeof(i64) == 8, "primitive_types.h: i64 must be 8 bytes");
static_assert(sizeof(u64) == 8, "primitive_types.h: u64 must be 8 bytes");
static_assert(sizeof(f32) == 4, "primitive_types.h: f32 must be 4 bytes");
static_assert(sizeof(f64) == 8, "primitive_types.h: f64 must be 8 bytes");
static_assert(sizeof(b8) == 1, "primitive_types.h: b8 must be 1 byte");
static_assert(sizeof(b16) == 2, "primitive_types.h: b16 must be 2 bytes");
static_assert(sizeof(b32) == 4, "primitive_types.h: b32 must be 4 bytes");
static_assert(sizeof(b64) == 8, "primitive_types.h: b64 must be 8 bytes");
static_assert(sizeof(c8) == 1, "primitive_types.h: c8 must be 1 byte");
static_assert(sizeof(c16) == 2, "primitive_types.h: c16 must be 2 bytes");
static_assert(sizeof(c32) == 4, "primitive_types.h: c32 must be 4 bytes");

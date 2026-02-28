#pragma once

#include "fixed_array.h"

// #############################################################################
// Types
// #############################################################################

// Even if a string is not technically fixed, we still treat it as if it was a fixed array.
// This is because we care about the NULL character terminator, not the actual count of elements.
// I like using fixed strings because they avoid dynamic memory allocation, which is good for performance and safety.
// Just be aware that strings defined this way have a maximum capacity defined at compile time.
// For my needs, this is usually sufficient.
template <typename T, SZ MAX>
struct String : Fixed_Array<T, MAX>
{
  String() = default;
  String(const T* cstring);
};

template <typename T>
using String_View = View<T>;

// Nice constants for strings used conventionally.
constant_variable SZ STRING_SIZE_TYNY = 16;
constant_variable SZ STRING_SIZE_SHORT = 64;
constant_variable SZ STRING_SIZE_MEDIUM = 512;
constant_variable SZ STRING_SIZE_LONG = 1024;
constant_variable SZ STRING_SIZE_LARGE = 2048;

// Predefined strings for standard types (8 bit)
using String8_Tiny = String<C8, STRING_SIZE_TYNY>;
using String8_Short = String<C8, STRING_SIZE_SHORT>;
using String8_Medium = String<C8, STRING_SIZE_MEDIUM>;
using String8_Long = String<C8, STRING_SIZE_LONG>;
using String8_Large = String<C8, STRING_SIZE_LARGE>;

// Predefined strings for standard types (16 bit)
using String16_Tiny = String<C16, STRING_SIZE_TYNY>;
using String16_Short = String<C16, STRING_SIZE_SHORT>;
using String16_Medium = String<C16, STRING_SIZE_MEDIUM>;
using String16_Long = String<C16, STRING_SIZE_LONG>;
using String16_Large = String<C16, STRING_SIZE_LARGE>;

// Predefined strings for standard types (32 bit)
using String32_Tiny = String<C32, STRING_SIZE_TYNY>;
using String32_Short = String<C32, STRING_SIZE_SHORT>;
using String32_Medium = String<C32, STRING_SIZE_MEDIUM>;
using String32_Long = String<C32, STRING_SIZE_LONG>;
using String32_Large = String<C32, STRING_SIZE_LARGE>;

// Predefined string views for standard types
using String8_View = String_View<C8>;
using String16_View = String_View<C16>;
using String32_View = String_View<C32>;

// #############################################################################
// Basic utilities
// #############################################################################

template <typename T, SZ MAX>
function void string_clear(String<T, MAX>* string);

template <typename T, SZ MAX>
function BL string_is_empty(const String<T, MAX>* string);

template <typename T, SZ MAX>
function SZ string_get_length(const String<T, MAX>* string);

// #############################################################################
// Section: Append / Fill / Constructors
// #############################################################################

template <typename T, SZ MAX>
function BL string_append_char(String<T, MAX>* string, T character);

template <typename T, SZ MAX>
function void string_fill(String<T, MAX>* string, const T* src);

template <typename T, SZ MAX1, SZ MAX2>
function void string_fill(String<T, MAX1>* string, const String<T, MAX2>* src);

template <typename T, SZ MAX1, SZ MAX2>
function BL string_copy(String<T, MAX2>* string, const String<T, MAX1>* copy);

template <typename T, SZ MAX>
String<T, MAX>::String(const T* cstring);

template <typename T, SZ MAX>
function String<T, MAX> string_get_from_cstring(const T* string);

template <typename T, SZ MAX1, SZ MAX2>
function BL string_append_substring(String<T, MAX1>* string, const String<T, MAX2>* substring);

template <typename T, SZ MAX>
function BL string_append_substring(String<T, MAX>* string, const T* substring_c);

template <typename T, SZ MAX>
function BL string_append_substring(String<T, MAX>* string, String<T, MAX> substring_c);

template <typename T, SZ MAX>
function void string_truncate(String<T, MAX>* string, SZ length);

// #############################################################################
// Case conversion
// #############################################################################

template <typename T, SZ MAX>
function void string_tolower(String<T, MAX>* string);

template <typename T, SZ MAX>
function void string_toupper(String<T, MAX>* string);

// #############################################################################
// Formatting & Scanning
// #############################################################################

template <SZ MAX>
function BL string_vformat(String<C8, MAX>* string, const C8* format, va_list args);

template <SZ MAX>
function BL string_format(String<C8, MAX>* string, const C8* format, ...);

template <SZ MAX>
function BL string_append_format(String<C8, MAX>* string, const C8* format, ...);

template <SZ MAX>
function BL string_scan(const String<C8, MAX>* string, const C8* format, ...);

// #############################################################################
// Search - Characters
// #############################################################################

template <typename T, SZ MAX>
function const T* string_find_last_char(const String<T, MAX>* string, T character);

template <typename T, SZ MAX>
function const T* string_find_first_char(const String<T, MAX>* string, T character);

template <typename T, SZ MAX>
function T* string_find_last_char(String<T, MAX>* string, T character);

template <typename T, SZ MAX>
function T* string_find_first_char(String<T, MAX>* string, T character);

template <typename T, SZ MAX>
function U32x string_find_char_count(const String<T, MAX>* string, T character);

// #############################################################################
// Search - Substrings
// #############################################################################

template <typename T, SZ MAX>
function T* string_find_last_substring(String<T, MAX>* string, const T* substring);

template <typename T, SZ MAX>
function T* string_find_first_substring(String<T, MAX>* string, const T* substring);

template <typename T, SZ MAX>
function String<T, MAX> string_get_common_string(const String<T, MAX>* a, const String<T, MAX>* b);

// #############################################################################
// Replace & Remove
// #############################################################################

template <typename T, SZ MAX>
function U32x string_replace_all_chars(String<T, MAX>* string, T replaced, T replacer);

template <typename T, size_t MAX>
function U32x string_remove_all_chars(String<T, MAX>* string, T c);

template <typename T, size_t MAX>
function U32x string_remove_all_whitespace(String<T, MAX>* string);

template <typename T, size_t MAX>
function U32x string_replace_substring(String<T, MAX>* string, const C8* replaced, const C8* replacer);

template <typename T, size_t MAX>
function BL string_remove_prefix(String<T, MAX>* string, const C8* prefix);

template <typename T, size_t MAX>
function BL string_remove_suffix(String<T, MAX>* string, const C8* suffix);

// #############################################################################
// Comparison
// #############################################################################

template <typename T, SZ MAX1, SZ MAX2>
function BL string_compare_case_sensitive(const String<T, MAX2>* string_a, const String<T, MAX1>* string_b);

template <typename T, SZ MAX1, SZ MAX2>
function BL string_compare_case_insensitive(const String<T, MAX2>* string_a, const String<T, MAX1>* string_b);

template <typename T, SZ MAX>
function BL string_compare_case_sensitive(const String<T, MAX>* string_a, const T* string_b_c);

template <typename T, SZ MAX>
function BL string_compare_case_insensitive(const String<T, MAX>* string_a, const T* string_b_c);

template <typename T, SZ MAX1, SZ MAX2>
function BL operator==(const String<T, MAX2>& string_a, const String<T, MAX1>& string_b);

template <typename T, SZ MAX1, SZ MAX2>
function BL operator!=(const String<T, MAX2>& string_a, const String<T, MAX1>& string_b);

template <typename T, SZ MAX1>
function BL operator==(const String<T, MAX1>& string_a, const T* string_b);

template <typename T, SZ MAX1>
function BL operator!=(const String<T, MAX1>& string_a, const T* string_b);

// #############################################################################
// Beautify
// #############################################################################

template <typename T, SZ MAX>
function String<T, MAX> string_beautify(const String<T, MAX>* string);

template <typename T, SZ MAX>
function String<T, MAX> string_beautify(const String<T, MAX> string);
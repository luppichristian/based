// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// Cross-platform physical keyboard keys aligned with the USB HID usage page.
// Backends can map these values to native scancodes without exposing
// platform-specific keycode enums through the public input API.
#define BASED_VKEY_LIST(X)            \
  X(VKEY_UNKNOWN, 0)                  \
  X(VKEY_A, 4)                        \
  X(VKEY_B, 5)                        \
  X(VKEY_C, 6)                        \
  X(VKEY_D, 7)                        \
  X(VKEY_E, 8)                        \
  X(VKEY_F, 9)                        \
  X(VKEY_G, 10)                       \
  X(VKEY_H, 11)                       \
  X(VKEY_I, 12)                       \
  X(VKEY_J, 13)                       \
  X(VKEY_K, 14)                       \
  X(VKEY_L, 15)                       \
  X(VKEY_M, 16)                       \
  X(VKEY_N, 17)                       \
  X(VKEY_O, 18)                       \
  X(VKEY_P, 19)                       \
  X(VKEY_Q, 20)                       \
  X(VKEY_R, 21)                       \
  X(VKEY_S, 22)                       \
  X(VKEY_T, 23)                       \
  X(VKEY_U, 24)                       \
  X(VKEY_V, 25)                       \
  X(VKEY_W, 26)                       \
  X(VKEY_X, 27)                       \
  X(VKEY_Y, 28)                       \
  X(VKEY_Z, 29)                       \
  X(VKEY_DIGIT_1, 30)                 \
  X(VKEY_DIGIT_2, 31)                 \
  X(VKEY_DIGIT_3, 32)                 \
  X(VKEY_DIGIT_4, 33)                 \
  X(VKEY_DIGIT_5, 34)                 \
  X(VKEY_DIGIT_6, 35)                 \
  X(VKEY_DIGIT_7, 36)                 \
  X(VKEY_DIGIT_8, 37)                 \
  X(VKEY_DIGIT_9, 38)                 \
  X(VKEY_DIGIT_0, 39)                 \
  X(VKEY_RETURN, 40)                  \
  X(VKEY_ESCAPE, 41)                  \
  X(VKEY_BACKSPACE, 42)               \
  X(VKEY_TAB, 43)                     \
  X(VKEY_SPACE, 44)                   \
  X(VKEY_MINUS, 45)                   \
  X(VKEY_EQUALS, 46)                  \
  X(VKEY_LEFT_BRACKET, 47)            \
  X(VKEY_RIGHT_BRACKET, 48)           \
  X(VKEY_BACKSLASH, 49)               \
  X(VKEY_NON_US_HASH, 50)             \
  X(VKEY_SEMICOLON, 51)               \
  X(VKEY_APOSTROPHE, 52)              \
  X(VKEY_GRAVE, 53)                   \
  X(VKEY_COMMA, 54)                   \
  X(VKEY_PERIOD, 55)                  \
  X(VKEY_SLASH, 56)                   \
  X(VKEY_CAPS_LOCK, 57)               \
  X(VKEY_F1, 58)                      \
  X(VKEY_F2, 59)                      \
  X(VKEY_F3, 60)                      \
  X(VKEY_F4, 61)                      \
  X(VKEY_F5, 62)                      \
  X(VKEY_F6, 63)                      \
  X(VKEY_F7, 64)                      \
  X(VKEY_F8, 65)                      \
  X(VKEY_F9, 66)                      \
  X(VKEY_F10, 67)                     \
  X(VKEY_F11, 68)                     \
  X(VKEY_F12, 69)                     \
  X(VKEY_PRINT_SCREEN, 70)            \
  X(VKEY_SCROLL_LOCK, 71)             \
  X(VKEY_PAUSE, 72)                   \
  X(VKEY_INSERT, 73)                  \
  X(VKEY_HOME, 74)                    \
  X(VKEY_PAGE_UP, 75)                 \
  X(VKEY_DELETE, 76)                  \
  X(VKEY_END, 77)                     \
  X(VKEY_PAGE_DOWN, 78)               \
  X(VKEY_RIGHT, 79)                   \
  X(VKEY_LEFT, 80)                    \
  X(VKEY_DOWN, 81)                    \
  X(VKEY_UP, 82)                      \
  X(VKEY_NUM_LOCK, 83)                \
  X(VKEY_KP_DIVIDE, 84)               \
  X(VKEY_KP_MULTIPLY, 85)             \
  X(VKEY_KP_MINUS, 86)                \
  X(VKEY_KP_PLUS, 87)                 \
  X(VKEY_KP_ENTER, 88)                \
  X(VKEY_KP_1, 89)                    \
  X(VKEY_KP_2, 90)                    \
  X(VKEY_KP_3, 91)                    \
  X(VKEY_KP_4, 92)                    \
  X(VKEY_KP_5, 93)                    \
  X(VKEY_KP_6, 94)                    \
  X(VKEY_KP_7, 95)                    \
  X(VKEY_KP_8, 96)                    \
  X(VKEY_KP_9, 97)                    \
  X(VKEY_KP_0, 98)                    \
  X(VKEY_KP_PERIOD, 99)               \
  X(VKEY_NON_US_BACKSLASH, 100)       \
  X(VKEY_APPLICATION, 101)            \
  X(VKEY_POWER, 102)                  \
  X(VKEY_KP_EQUALS, 103)              \
  X(VKEY_F13, 104)                    \
  X(VKEY_F14, 105)                    \
  X(VKEY_F15, 106)                    \
  X(VKEY_F16, 107)                    \
  X(VKEY_F17, 108)                    \
  X(VKEY_F18, 109)                    \
  X(VKEY_F19, 110)                    \
  X(VKEY_F20, 111)                    \
  X(VKEY_F21, 112)                    \
  X(VKEY_F22, 113)                    \
  X(VKEY_F23, 114)                    \
  X(VKEY_F24, 115)                    \
  X(VKEY_EXECUTE, 116)                \
  X(VKEY_HELP, 117)                   \
  X(VKEY_MENU, 118)                   \
  X(VKEY_SELECT, 119)                 \
  X(VKEY_STOP, 120)                   \
  X(VKEY_AGAIN, 121)                  \
  X(VKEY_UNDO, 122)                   \
  X(VKEY_CUT, 123)                    \
  X(VKEY_COPY, 124)                   \
  X(VKEY_PASTE, 125)                  \
  X(VKEY_FIND, 126)                   \
  X(VKEY_MUTE, 127)                   \
  X(VKEY_VOLUME_UP, 128)              \
  X(VKEY_VOLUME_DOWN, 129)            \
  X(VKEY_KP_COMMA, 133)               \
  X(VKEY_KP_EQUALS_AS400, 134)        \
  X(VKEY_INTERNATIONAL_1, 135)        \
  X(VKEY_INTERNATIONAL_2, 136)        \
  X(VKEY_INTERNATIONAL_3, 137)        \
  X(VKEY_INTERNATIONAL_4, 138)        \
  X(VKEY_INTERNATIONAL_5, 139)        \
  X(VKEY_INTERNATIONAL_6, 140)        \
  X(VKEY_INTERNATIONAL_7, 141)        \
  X(VKEY_INTERNATIONAL_8, 142)        \
  X(VKEY_INTERNATIONAL_9, 143)        \
  X(VKEY_LANG_1, 144)                 \
  X(VKEY_LANG_2, 145)                 \
  X(VKEY_LANG_3, 146)                 \
  X(VKEY_LANG_4, 147)                 \
  X(VKEY_LANG_5, 148)                 \
  X(VKEY_LANG_6, 149)                 \
  X(VKEY_LANG_7, 150)                 \
  X(VKEY_LANG_8, 151)                 \
  X(VKEY_LANG_9, 152)                 \
  X(VKEY_ALT_ERASE, 153)              \
  X(VKEY_SYS_REQ, 154)                \
  X(VKEY_CANCEL, 155)                 \
  X(VKEY_CLEAR, 156)                  \
  X(VKEY_PRIOR, 157)                  \
  X(VKEY_RETURN_2, 158)               \
  X(VKEY_SEPARATOR, 159)              \
  X(VKEY_OUT, 160)                    \
  X(VKEY_OPER, 161)                   \
  X(VKEY_CLEAR_AGAIN, 162)            \
  X(VKEY_CR_SEL, 163)                 \
  X(VKEY_EX_SEL, 164)                 \
  X(VKEY_KP_00, 176)                  \
  X(VKEY_KP_000, 177)                 \
  X(VKEY_THOUSANDS_SEPARATOR, 178)    \
  X(VKEY_DECIMAL_SEPARATOR, 179)      \
  X(VKEY_CURRENCY_UNIT, 180)          \
  X(VKEY_CURRENCY_SUBUNIT, 181)       \
  X(VKEY_KP_LEFT_PAREN, 182)          \
  X(VKEY_KP_RIGHT_PAREN, 183)         \
  X(VKEY_KP_LEFT_BRACE, 184)          \
  X(VKEY_KP_RIGHT_BRACE, 185)         \
  X(VKEY_KP_TAB, 186)                 \
  X(VKEY_KP_BACKSPACE, 187)           \
  X(VKEY_KP_A, 188)                   \
  X(VKEY_KP_B, 189)                   \
  X(VKEY_KP_C, 190)                   \
  X(VKEY_KP_D, 191)                   \
  X(VKEY_KP_E, 192)                   \
  X(VKEY_KP_F, 193)                   \
  X(VKEY_KP_XOR, 194)                 \
  X(VKEY_KP_POWER, 195)               \
  X(VKEY_KP_PERCENT, 196)             \
  X(VKEY_KP_LESS, 197)                \
  X(VKEY_KP_GREATER, 198)             \
  X(VKEY_KP_AMPERSAND, 199)           \
  X(VKEY_KP_DOUBLE_AMPERSAND, 200)    \
  X(VKEY_KP_VERTICAL_BAR, 201)        \
  X(VKEY_KP_DOUBLE_VERTICAL_BAR, 202) \
  X(VKEY_KP_COLON, 203)               \
  X(VKEY_KP_HASH, 204)                \
  X(VKEY_KP_SPACE, 205)               \
  X(VKEY_KP_AT, 206)                  \
  X(VKEY_KP_EXCLAMATION, 207)         \
  X(VKEY_KP_MEM_STORE, 208)           \
  X(VKEY_KP_MEM_RECALL, 209)          \
  X(VKEY_KP_MEM_CLEAR, 210)           \
  X(VKEY_KP_MEM_ADD, 211)             \
  X(VKEY_KP_MEM_SUBTRACT, 212)        \
  X(VKEY_KP_MEM_MULTIPLY, 213)        \
  X(VKEY_KP_MEM_DIVIDE, 214)          \
  X(VKEY_KP_PLUS_MINUS, 215)          \
  X(VKEY_KP_CLEAR, 216)               \
  X(VKEY_KP_CLEAR_ENTRY, 217)         \
  X(VKEY_KP_BINARY, 218)              \
  X(VKEY_KP_OCTAL, 219)               \
  X(VKEY_KP_DECIMAL, 220)             \
  X(VKEY_KP_HEXADECIMAL, 221)         \
  X(VKEY_LEFT_CTRL, 224)              \
  X(VKEY_LEFT_SHIFT, 225)             \
  X(VKEY_LEFT_ALT, 226)               \
  X(VKEY_LEFT_GUI, 227)               \
  X(VKEY_RIGHT_CTRL, 228)             \
  X(VKEY_RIGHT_SHIFT, 229)            \
  X(VKEY_RIGHT_ALT, 230)              \
  X(VKEY_RIGHT_GUI, 231)              \
  X(VKEY_MODE, 257)                   \
  X(VKEY_SLEEP, 258)                  \
  X(VKEY_WAKE, 259)                   \
  X(VKEY_CHANNEL_INCREMENT, 260)      \
  X(VKEY_CHANNEL_DECREMENT, 261)      \
  X(VKEY_MEDIA_PLAY, 262)             \
  X(VKEY_MEDIA_PAUSE, 263)            \
  X(VKEY_MEDIA_RECORD, 264)           \
  X(VKEY_MEDIA_FAST_FORWARD, 265)     \
  X(VKEY_MEDIA_REWIND, 266)           \
  X(VKEY_MEDIA_NEXT_TRACK, 267)       \
  X(VKEY_MEDIA_PREVIOUS_TRACK, 268)   \
  X(VKEY_MEDIA_STOP, 269)             \
  X(VKEY_MEDIA_EJECT, 270)            \
  X(VKEY_MEDIA_PLAY_PAUSE, 271)       \
  X(VKEY_MEDIA_SELECT, 272)           \
  X(VKEY_AC_NEW, 273)                 \
  X(VKEY_AC_OPEN, 274)                \
  X(VKEY_AC_CLOSE, 275)               \
  X(VKEY_AC_EXIT, 276)                \
  X(VKEY_AC_SAVE, 277)                \
  X(VKEY_AC_PRINT, 278)               \
  X(VKEY_AC_PROPERTIES, 279)          \
  X(VKEY_AC_SEARCH, 280)              \
  X(VKEY_AC_HOME, 281)                \
  X(VKEY_AC_BACK, 282)                \
  X(VKEY_AC_FORWARD, 283)             \
  X(VKEY_AC_STOP, 284)                \
  X(VKEY_AC_REFRESH, 285)             \
  X(VKEY_AC_BOOKMARKS, 286)           \
  X(VKEY_SOFT_LEFT, 287)              \
  X(VKEY_SOFT_RIGHT, 288)             \
  X(VKEY_CALL, 289)                   \
  X(VKEY_END_CALL, 290)

typedef enum vkey {
#define BASED_VKEY_ENUM(name, value) name = (value),
  BASED_VKEY_LIST(BASED_VKEY_ENUM)
#undef BASED_VKEY_ENUM
} vkey;

func force_inline b32 vkey_is_valid(vkey key) {
  switch (key) {
#define BASED_VKEY_CASE(name, value) case name:
    BASED_VKEY_LIST(BASED_VKEY_CASE)
#undef BASED_VKEY_CASE
    return true;
    default:
      return false;
  }
}

// Key modifier bitmask compatible with backend events.
typedef enum keymod {
  KEYMOD_NONE = 0x0000U,
  KEYMOD_LSHIFT = 0x0001U,
  KEYMOD_RSHIFT = 0x0002U,
  KEYMOD_LEVEL5 = 0x0004U,
  KEYMOD_LCTRL = 0x0040U,
  KEYMOD_RCTRL = 0x0080U,
  KEYMOD_LALT = 0x0100U,
  KEYMOD_RALT = 0x0200U,
  KEYMOD_LGUI = 0x0400U,
  KEYMOD_RGUI = 0x0800U,
  KEYMOD_NUM = 0x1000U,
  KEYMOD_CAPS = 0x2000U,
  KEYMOD_MODE = 0x4000U,
  KEYMOD_SCROLL = 0x8000U,
  KEYMOD_SHIFT = KEYMOD_LSHIFT | KEYMOD_RSHIFT,
  KEYMOD_CTRL = KEYMOD_LCTRL | KEYMOD_RCTRL,
  KEYMOD_ALT = KEYMOD_LALT | KEYMOD_RALT,
  KEYMOD_GUI = KEYMOD_LGUI | KEYMOD_RGUI,
} keymod;

// =========================================================================
c_end;
// =========================================================================
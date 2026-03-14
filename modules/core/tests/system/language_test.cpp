// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(system_language_test, query) {
  language_info info = {0};
  b32 result = language_query(&info);
  EXPECT_NE(0, result);
}

TEST(system_language_test, desktop_language) {
  language_info info = {0};
  b32 result = language_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.desktop_language), 0U);
}

TEST(system_language_test, keyboard_language) {
  language_info info = {0};
  b32 result = language_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.keyboard_language), 0U);
  EXPECT_GT(cstr8_len(info.keyboard_layout_name), 0U);
}

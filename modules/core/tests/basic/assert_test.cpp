// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  global_var i32 basic_assert_hook_calls = 0;
  global_var assert_mode basic_assert_hook_mode = ASSERT_MODE_IGNORE;
  global_var cstr8 basic_assert_hook_msg = NULL;
  global_var u32 basic_assert_hook_line = 0;

  func void basic_assert_test_hook(assert_mode mode, cstr8 msg, callsite site, void* user_data) {
    i32* hook_counter = (i32*)user_data;
    if (hook_counter != NULL) {
      *hook_counter += 1;
    }
    basic_assert_hook_calls += 1;
    basic_assert_hook_mode = mode;
    basic_assert_hook_msg = msg;
    basic_assert_hook_line = site.line;
  }
}  // namespace

TEST(basic_assert_test, set_mode_roundtrip_and_ignore_invalid_value) {
  assert_mode prev_mode = assert_get_mode();

  assert_set_mode(ASSERT_MODE_IGNORE);
  EXPECT_EQ(assert_get_mode(), ASSERT_MODE_IGNORE);

  assert_set_mode((assert_mode)999);
  EXPECT_EQ(assert_get_mode(), ASSERT_MODE_IGNORE);

  assert_set_mode(prev_mode);
}
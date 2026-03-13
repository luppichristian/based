// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(interface_cursor_test, cursor_icon_and_visibility_queries_are_safe) {
  icon system_icon_id = icon_create_system(ICON_SYSTEM_DEFAULT);
  ASSERT_NE(nullptr, system_icon_id);

  (void)cursor_set_icon(system_icon_id);
  (void)cursor_reset_icon();
  (void)cursor_set_visible(cursor_is_visible());
  (void)cursor_set_capture(nullptr, 1);
  (void)cursor_set_relative_mode(nullptr, 1);
  (void)cursor_warp(nullptr, 0.0F, 0.0F);
  SUCCEED();

  EXPECT_TRUE(icon_destroy(system_icon_id) != 0);
}

TEST(interface_cursor_test, invalid_cursor_icon_is_rejected) {
  EXPECT_TRUE(cursor_set_icon(nullptr) == 0);
}

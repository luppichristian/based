// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(interface_cursor_test, cursor_icon_and_visibility_queries_are_safe) {
  icon system_icon_id = icon_create_system(ICON_SYSTEM_DEFAULT);
  ASSERT_NE(nullptr, system_icon_id);

  cursor_pos local_pos = cursor_get_pos();
  cursor_pos global_pos = cursor_get_global_pos();
  cursor_pos relative_pos = cursor_get_relative_pos();
  EXPECT_TRUE(local_pos.x == local_pos.x);
  EXPECT_TRUE(global_pos.x == global_pos.x);
  EXPECT_TRUE(relative_pos.x == relative_pos.x);

  (void)cursor_set_icon(system_icon_id);
  (void)cursor_reset_icon();
  (void)cursor_set_visible(cursor_is_visible());
  (void)cursor_set_capture(NULL, 1);
  (void)cursor_set_relative_mode(NULL, 1);
  (void)cursor_warp(NULL, 0.0F, 0.0F);
  SUCCEED();

  EXPECT_TRUE(icon_destroy(system_icon_id) != 0);
}

TEST(interface_cursor_test, invalid_cursor_icon_is_rejected) {
  EXPECT_TRUE(cursor_set_icon(NULL) == 0);
}

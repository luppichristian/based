// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(interface_window_test, invalid_window_id_paths_return_failure) {
  EXPECT_TRUE(window_id_is_valid(nullptr) == 0);
  EXPECT_EQ((up)456, window_to_native_id(window_from_native_id(456)));

  i32 xpos_val = 0;
  i32 ypos_val = 0;
  i32 widt_val = 0;
  i32 high_val = 0;
  monitor mon_id = nullptr;
  const u8 white_pixels[4] = {255, 255, 255, 255};
  icon icon_id = icon_create_rgba(1, 1, white_pixels, 0, 0);

  EXPECT_TRUE(window_is_valid(nullptr) == 0);
  EXPECT_TRUE(window_destroy(nullptr) == 0);
  EXPECT_TRUE(window_is_hidden(nullptr) == 0);
  EXPECT_TRUE(window_show(nullptr) == 0);
  EXPECT_TRUE(window_hide(nullptr) == 0);
  EXPECT_TRUE(window_set_pos(nullptr, 0, 0) == 0);
  EXPECT_TRUE(window_get_pos(nullptr, &xpos_val, &ypos_val) == 0);
  EXPECT_TRUE(window_set_size(nullptr, 10, 10) == 0);
  EXPECT_TRUE(window_get_size(nullptr, &widt_val, &high_val) == 0);
  EXPECT_TRUE(window_set_mode(nullptr, WINDOW_MODE_FULLSCREEN) == 0);
  EXPECT_EQ(WINDOW_MODE_WINDOWED, window_get_mode(nullptr));
  EXPECT_TRUE(window_is_minimized(nullptr) == 0);
  EXPECT_TRUE(window_is_maximized(nullptr) == 0);
  EXPECT_TRUE(window_has_input_focus(nullptr) == 0);
  EXPECT_TRUE(window_has_mouse_focus(nullptr) == 0);
  EXPECT_TRUE(window_is_topmost(nullptr) == 0);
  EXPECT_TRUE(window_set_title(nullptr, "x") == 0);
  EXPECT_EQ(nullptr, window_get_title(nullptr));
  EXPECT_TRUE(window_set_icon(nullptr, icon_id) == 0);
  EXPECT_TRUE(window_set_topmost(nullptr, 1) == 0);
  EXPECT_TRUE(window_get_monitor(nullptr, &mon_id) == 0);
  EXPECT_TRUE(window_get_creation_monitor(nullptr, &mon_id) == 0);
  EXPECT_TRUE(window_center_in_monitor(nullptr, nullptr, WINDOW_CENTER_AXIS_BOTH) == 0);

  ASSERT_TRUE(icon_id != nullptr);
  EXPECT_TRUE(icon_destroy(icon_id) != 0);
}

TEST(interface_window_test, create_modify_and_destroy_window_when_available) {
#if defined(_WIN32)
  GTEST_SKIP() << "window creation is unstable on this target";
#endif
  window win_id = window_create("based test", 320, 200, WINDOW_MODE_WINDOWED, 0);
  if (win_id == nullptr) {
    GTEST_SKIP() << "window backend not available";
  }

  ASSERT_TRUE(window_is_valid(win_id) != 0);
  EXPECT_EQ(WINDOW_MODE_WINDOWED, window_get_mode(win_id));
  EXPECT_TRUE(window_is_hidden(win_id) == 0);
  EXPECT_TRUE(window_set_title(win_id, "based test renamed") != 0);
  cstr8 title_val = window_get_title(win_id);
  ASSERT_NE(nullptr, title_val);
  EXPECT_STREQ("based test renamed", title_val);
  EXPECT_TRUE(window_is_minimized(win_id) == 0);
  EXPECT_TRUE(window_is_maximized(win_id) == 0);
  EXPECT_TRUE(window_has_input_focus(win_id) == 0 || window_has_input_focus(win_id) == 1);
  EXPECT_TRUE(window_has_mouse_focus(win_id) == 0 || window_has_mouse_focus(win_id) == 1);
  EXPECT_TRUE(window_is_topmost(win_id) == 0);
  EXPECT_TRUE(window_set_topmost(win_id, 1) != 0);
  EXPECT_TRUE(window_is_topmost(win_id) != 0);
  EXPECT_TRUE(window_set_topmost(win_id, 0) != 0);
  EXPECT_TRUE(window_is_topmost(win_id) == 0);
  EXPECT_TRUE(window_hide(win_id) != 0);
  EXPECT_TRUE(window_is_hidden(win_id) != 0);
  EXPECT_TRUE(window_show(win_id) != 0);
  EXPECT_TRUE(window_is_hidden(win_id) == 0);

  monitor current_monitor_id = nullptr;
  monitor creation_monitor_id = nullptr;
  EXPECT_TRUE(window_get_monitor(win_id, &current_monitor_id) != 0);
  EXPECT_TRUE(window_get_creation_monitor(win_id, &creation_monitor_id) != 0);
  EXPECT_TRUE(monitor_id_is_valid(current_monitor_id) != 0);
  EXPECT_TRUE(monitor_id_is_valid(creation_monitor_id) != 0);
  EXPECT_TRUE(window_center_in_monitor(win_id, current_monitor_id, WINDOW_CENTER_AXIS_BOTH) != 0);

  const u8 green_pixels[4] = {0, 255, 0, 255};
  icon icon_id = icon_create_rgba(1, 1, green_pixels, 0, 0);
  ASSERT_NE(nullptr, icon_id);
  EXPECT_TRUE(window_set_icon(win_id, icon_id) != 0);
  EXPECT_TRUE(icon_destroy(icon_id) != 0);

  EXPECT_TRUE(window_destroy(win_id) != 0);
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(interface_icon_test, rgba_and_system_icons_report_expected_metadata) {
  const u8 pixels[16] = {
      255,
      0,
      0,
      255,
      0,
      255,
      0,
      255,
      0,
      0,
      255,
      255,
      255,
      255,
      255,
      255,
  };

  icon rgba_icon_id = icon_create_rgba(2, 2, pixels, 1, 1);
  ASSERT_NE(nullptr, rgba_icon_id);
  EXPECT_TRUE(icon_is_rgba(rgba_icon_id) != 0);
  EXPECT_TRUE(icon_is_system(rgba_icon_id) == 0);

  i32 width = 0;
  i32 height = 0;
  i32 hot_xpos = 0;
  i32 hot_ypos = 0;
  EXPECT_TRUE(icon_get_size(rgba_icon_id, &width, &height) != 0);
  EXPECT_TRUE(icon_get_hotspot(rgba_icon_id, &hot_xpos, &hot_ypos) != 0);
  EXPECT_EQ(2, width);
  EXPECT_EQ(2, height);
  EXPECT_EQ(1, hot_xpos);
  EXPECT_EQ(1, hot_ypos);
  EXPECT_TRUE(icon_destroy(rgba_icon_id) != 0);

  icon system_icon_id = icon_create_system(ICON_SYSTEM_POINTER);
  ASSERT_NE(nullptr, system_icon_id);
  EXPECT_TRUE(icon_is_system(system_icon_id) != 0);
  EXPECT_TRUE(icon_is_rgba(system_icon_id) == 0);

  icon_system system_icon = ICON_SYSTEM_DEFAULT;
  EXPECT_TRUE(icon_get_system(system_icon_id, &system_icon) != 0);
  EXPECT_EQ(ICON_SYSTEM_POINTER, system_icon);
  EXPECT_TRUE(icon_destroy(system_icon_id) != 0);
}

TEST(interface_icon_test, invalid_icon_queries_are_safe) {
  i32 value = 0;
  icon_system system_icon = ICON_SYSTEM_DEFAULT;

  EXPECT_EQ(nullptr, icon_create_rgba(0, 0, NULL, 0, 0));
  EXPECT_EQ(nullptr, icon_create_system((icon_system)99));
  EXPECT_TRUE(icon_destroy(NULL) == 0);
  EXPECT_TRUE(icon_get_size(NULL, &value, &value) == 0);
  EXPECT_TRUE(icon_get_hotspot(NULL, &value, &value) == 0);
  EXPECT_TRUE(icon_get_system(NULL, &system_icon) == 0);
}

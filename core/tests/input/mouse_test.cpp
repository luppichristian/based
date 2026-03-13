// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_mouse_test, invalid_buttons_and_state_queries_are_safe) {
  EXPECT_TRUE(mouse_is_button_down((mouse_button)-1) == 0);
  EXPECT_TRUE(mouse_is_button_down((mouse_button)9) == 0);

  mouse_state state = mouse_get_state();
  EXPECT_GE(state, 0U);
}

TEST(input_mouse_test, primary_device_id_query_is_consistent) {
  device_id out_id = {};
  b32 has_primary = mouse_get_primary_device_id(&out_id);
  if (has_primary != 0) {
    EXPECT_TRUE(device_id_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_MOUSE, out_id.type);
  }
}

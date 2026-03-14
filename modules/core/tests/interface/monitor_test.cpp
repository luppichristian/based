// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(interface_monitor_test, id_conversion_and_invalid_queries) {
  monitor mon_id = monitor_from_native_id(123);
  EXPECT_TRUE(monitor_id_is_valid(mon_id) != 0);
  EXPECT_EQ((up)123, monitor_to_native_id(mon_id));
  EXPECT_TRUE(monitor_id_is_valid(NULL) == 0);

  r2_i32 out_rect = {};
  monitor_mode out_mode = {};
  EXPECT_TRUE(monitor_get_mode(NULL, 0, &out_mode) == 0);
  EXPECT_TRUE(monitor_get_current_mode(NULL, &out_mode) == 0);
  EXPECT_TRUE(monitor_get_desktop_mode(NULL, &out_mode) == 0);
  EXPECT_EQ(0.0F, monitor_get_refresh_rate(NULL));
  EXPECT_EQ(1.0F, monitor_get_content_scale(NULL));
  EXPECT_EQ(MONITOR_ORIENTATION_UNKNOWN, monitor_get_orientation(NULL));
}

TEST(interface_monitor_test, count_and_primary_monitor_queries) {
  sz count_val = monitor_get_total_count();
  if (count_val == 0) {
    EXPECT_EQ(nullptr, monitor_get_from_idx(0));
  } else {
    monitor out_id = monitor_get_from_idx(0);
    EXPECT_NE(nullptr, out_id);
    EXPECT_TRUE(monitor_id_is_valid(out_id) != 0);
    EXPECT_NE(nullptr, monitor_get_name(out_id));
  }

  monitor prim_id = monitor_get_primary_id();
  if (monitor_id_is_valid(prim_id) != 0) {
    EXPECT_NE(nullptr, monitor_get_name(prim_id));
  }
}

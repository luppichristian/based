// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(system_hardware_info_test, query) {
  hardware_info info = {0};
  b32 result = hardware_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(info.cpu.logical_core_count, 0U);
  EXPECT_GT(info.memory_total, 0U);
}

TEST(system_hardware_info_test, component_counts) {
  hardware_info info = {0};
  b32 result = hardware_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.monitor_count, 0U);
  EXPECT_GE(info.keyboard_count, 0U);
  EXPECT_GE(info.mouse_count, 0U);
  EXPECT_GE(info.gamepad_count, 0U);
}

TEST(system_hardware_info_test, strings_are_initialized) {
  hardware_info info = {0};
  b32 result = hardware_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.video_driver_name), 0U);
}

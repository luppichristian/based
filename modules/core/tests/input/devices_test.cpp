// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(input_devices_test, type_names_are_stable) {
  EXPECT_STREQ("keyboard", devices_get_type_name(DEVICE_TYPE_KEYBOARD));
  EXPECT_STREQ("audio", devices_get_type_name(DEVICE_TYPE_AUDIO));
  EXPECT_STREQ("unknown", devices_get_type_name(DEVICE_TYPE_UNKNOWN));
}

TEST(input_devices_test, invalid_handles_are_reported_as_disconnected) {
  device bad_id = NULL;
  EXPECT_TRUE(device_is_valid(bad_id) == 0);
  EXPECT_TRUE(devices_is_connected(bad_id) == 0);
  EXPECT_TRUE(devices_get_info(bad_id, NULL) == 0);

  sz key_count = devices_get_count(DEVICE_TYPE_KEYBOARD);
  device out_id = NULL;
  if (key_count == 0) {
    EXPECT_EQ(nullptr, devices_get_device(DEVICE_TYPE_KEYBOARD, 0));
  } else {
    out_id = devices_get_device(DEVICE_TYPE_KEYBOARD, 0);
    EXPECT_NE(nullptr, out_id);
    EXPECT_TRUE(device_is_valid(out_id) != 0);
  }
}

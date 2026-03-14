// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(interface_dialog_test, validation_failures_do_not_require_user_interaction) {
  EXPECT_TRUE(dialog_show_message_box(NULL, NULL, NULL) == 0);

  dialog_message_box bad_box = {};
  bad_box.title = "Invalid";
  bad_box.message = "Missing buttons array";
  bad_box.button_count = 1;
  bad_box.buttons = NULL;
  EXPECT_TRUE(dialog_show_message_box(NULL, &bad_box, NULL) == 0);
}

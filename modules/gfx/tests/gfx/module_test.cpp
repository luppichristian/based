// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(gfx_module_test, reports_scaffold_state) {
  EXPECT_TRUE(gfx_module_is_available());
  EXPECT_STREQ(gfx_module_get_name(), "gfx");
}

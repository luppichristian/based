// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(interface_dpi_test, default_metrics_and_invalid_queries_are_safe) {
  dpi_metrics metrics = dpi_metrics_make_default();
  EXPECT_FLOAT_EQ(1.0f, metrics.content_scale);
  EXPECT_FLOAT_EQ(1.0f, metrics.pixel_density);
  EXPECT_FLOAT_EQ(1.0f, metrics.display_scale);

  EXPECT_TRUE(dpi_get_monitor_metrics(NULL, &metrics) == 0);
  EXPECT_TRUE(dpi_get_window_metrics(NULL, &metrics) == 0);
  EXPECT_FLOAT_EQ(6.0f, dpi_scale_f32(3.0f, 2.0f));
  EXPECT_EQ(6, dpi_scale_i32(3, 2.0f));
}

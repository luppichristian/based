// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_stacktrace_test, capture_handles_null_or_zero_capacity) {
  EXPECT_EQ(stacktrace_capture(nullptr, 0, 0), 0);
}

TEST(basic_stacktrace_test, capture_does_not_exceed_requested_capacity) {
  stacktrace_frame frames[8] = {};
  sz captured_count = stacktrace_capture(frames, 8, 0);
  EXPECT_LE(captured_count, (sz)8);
}

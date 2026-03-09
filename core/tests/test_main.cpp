// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

func b32 run(cmdline cmdl) {
  int argc = cmdl.count;
  char** argv = cmdl.args;
  testing::InitGoogleTest(&argc, argv);
  int test_result = RUN_ALL_TESTS();
  return test_result == 0;
}
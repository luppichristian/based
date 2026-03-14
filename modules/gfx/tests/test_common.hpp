// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#if defined BASED_PROFILER_ENABLED
#  error "test_common.hpp: BASED_PROFILER_ENABLED is not supported for unit tests"
#endif

#include <gtest/gtest.h>

#define ENTRY_TYPE_RUN
#include <based_gfx.h>
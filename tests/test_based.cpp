// MIT License
// Copyright (c) 2026 Christian Luppi

#include <gtest/gtest.h>

extern "C" {
#include "based.h"
}

TEST(based_add, positive_numbers)
{
    EXPECT_EQ(based_add(2, 3), 5);
}

TEST(based_add, negative_numbers)
{
    EXPECT_EQ(based_add(-4, -6), -10);
}

TEST(based_add, zero)
{
    EXPECT_EQ(based_add(0, 0), 0);
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include <gtest/gtest.h>

extern "C" {
#include "filesystem/path.h"
}

TEST(path, join_normalize_and_components) {
  path value = path_from_cstr("root\\folder");
  path append_value = path_from_cstr("file.txt");
  path ext_value;
  path name_value;
  path base_value;
  path dir_value;

  path_append(&value, &append_value);
  ext_value = path_get_extension(&value);
  name_value = path_get_name(&value);
  base_value = path_get_basename(&value);
  dir_value = path_get_directory(&value);

  EXPECT_STREQ(value.buf, "root/folder/file.txt");
  EXPECT_TRUE(path_ends_with(&value, ".txt"));
  EXPECT_FALSE(path_is_absolute(&value));
  EXPECT_STREQ(ext_value.buf, ".txt");
  EXPECT_STREQ(name_value.buf, "file.txt");
  EXPECT_STREQ(base_value.buf, "file");
  EXPECT_STREQ(dir_value.buf, "root/folder");
}

TEST(path, remove_helpers) {
  path value = path_from_cstr("C:\\alpha\\beta\\gamma.tar.gz");
  path trailing_value = path_from_cstr("folder/");

  path_normalize(&value);
  EXPECT_STREQ(value.buf, "C:/alpha/beta/gamma.tar.gz");
  EXPECT_TRUE(path_is_absolute(&value));

  path_remove_extension(&value);
  EXPECT_STREQ(value.buf, "C:/alpha/beta/gamma.tar");

  path_remove_name(&value);
  EXPECT_STREQ(value.buf, "C:/alpha/beta");

  path_remove_directory(&value);
  EXPECT_STREQ(value.buf, "beta");

  path_remove_directory(&trailing_value);
  EXPECT_STREQ(trailing_value.buf, "folder");
}

TEST(path, common_prefix_uses_directory_boundary) {
  path values[3];
  path common;

  values[0] = path_from_cstr("/alpha/bravo/file.txt");
  values[1] = path_from_cstr("/alpha/brick/other.txt");
  values[2] = path_from_cstr("/alpha/breeze/final.txt");
  common = path_get_common(values, size_of(values) / size_of(values[0]));

  EXPECT_STREQ(common.buf, "/alpha");
}

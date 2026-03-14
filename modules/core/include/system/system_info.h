// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct system_info {
  // Operating system family name such as "Windows", "Linux", or "Darwin".
  str8_short os_name;

  // OS release/build string returned by the active platform backend.
  str8_short os_version;

  // Compile-time architecture label for the running build target.
  str8_short architecture_name;

  // Host/computer/network node name for the current machine.
  str8_short computer_name;

  // Effective user/account name for the current process owner.
  str8_short user_name;

  // Home-directory path resolved from the environment or platform user database.
  str8_short user_home;

  // Native virtual-memory page size used by the operating system.
  sz page_size;

  // Minimum granularity for certain system allocations or mappings.
  // On Unix this currently matches page_size; on Windows it can be larger.
  sz allocation_granularity;
} system_info;

// Queries coarse OS, machine, and active-user information for the current host.
func b32 system_info_query(system_info* out_info);

// =========================================================================
c_end;
// =========================================================================

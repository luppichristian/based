// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/hardware_info.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "context/thread_ctx.h"
#include "input/devices.h"
#include "memory/memops.h"
#include "../sdl3_include.h"

func void hardware_info_copy_primary_device_name(device_type type, c8* dst, sz dst_cap) {
  profile_func_begin;
  if (dst == NULL || dst_cap == 0) {
    profile_func_end;
    return;
  }

  cstr8_clear(dst);
  if (devices_get_count(type) == 0) {
    profile_func_end;
    return;
  }

  device device_id = devices_get_device(type, 0);
  device_info info = {0};
  if (!device_is_valid(device_id) || !devices_get_info(device_id, &info)) {
    thread_log_debug("Failed to query primary device name type=%s", devices_get_type_name(type));
    profile_func_end;
    return;
  }

  cstr8_cpy(dst, dst_cap, info.name);
  profile_func_end;
}

func b32 hardware_info_query(hardware_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_info != NULL);

  mem_zero(out_info, size_of(*out_info));

  if (!cpu_info_query(&out_info->cpu)) {
    thread_log_error("Failed to query CPU info for hardware info");
    profile_func_end;
    return false;
  }

  system_runtime_info runtime_info = {0};
  if (!system_runtime_query(&runtime_info)) {
    thread_log_error("Failed to query runtime memory info for hardware info");
    profile_func_end;
    return false;
  }

  out_info->memory_total = runtime_info.memory_total;
  out_info->memory_available = runtime_info.memory_available;
  out_info->monitor_count = (u32)devices_get_count(DEVICE_TYPE_MONITOR);
  out_info->keyboard_count = (u32)devices_get_count(DEVICE_TYPE_KEYBOARD);
  out_info->mouse_count = (u32)devices_get_count(DEVICE_TYPE_MOUSE);
  out_info->gamepad_count = (u32)devices_get_count(DEVICE_TYPE_GAMEPAD);

  cstr8_cpy(out_info->video_driver_name, size_of(out_info->video_driver_name), "unknown");
  cstr8 video_driver_name = SDL_GetCurrentVideoDriver();
  if (video_driver_name != NULL) {
    cstr8_cpy(out_info->video_driver_name, size_of(out_info->video_driver_name), video_driver_name);
  }

  hardware_info_copy_primary_device_name(
      DEVICE_TYPE_KEYBOARD, out_info->primary_keyboard_name, size_of(out_info->primary_keyboard_name));
  hardware_info_copy_primary_device_name(
      DEVICE_TYPE_MOUSE, out_info->primary_mouse_name, size_of(out_info->primary_mouse_name));

  thread_log_trace("Queried hardware info monitors=%u keyboards=%u mice=%u",
                   out_info->monitor_count,
                   out_info->keyboard_count,
                   out_info->mouse_count);
  profile_func_end;
  return true;
}

// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/capture.h"

global_var input_key input_capture_keys[INPUT_CAPTURE_MAX_KEYS] = {0};
global_var b32 input_capture_used[INPUT_CAPTURE_MAX_KEYS] = {0};
global_var u32 input_capture_slot_epoch[INPUT_CAPTURE_MAX_KEYS] = {0};

func u32 input_capture_next_epoch(u32 value) {
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  return result;
}

func void input_capture_ensure_default_slot(void) {
  if (!input_capture_used[0]) {
    input_capture_keys[0] = INPUT_KEY_DEFAULT;
    input_capture_used[0] = 1;
    input_capture_slot_epoch[0] = input_capture_next_epoch(input_capture_slot_epoch[0]);
  }
}

func sz input_capture_find_slot(input_key key) {
  for (sz slot_idx = 0; slot_idx < INPUT_CAPTURE_MAX_KEYS; slot_idx += 1) {
    if (input_capture_used[slot_idx] && input_capture_keys[slot_idx] == key) {
      return slot_idx;
    }
  }

  return INPUT_CAPTURE_MAX_KEYS;
}

func sz input_capture_get_slot(input_key key) {
  input_capture_ensure_default_slot();

  sz found_idx = input_capture_find_slot(key);
  if (found_idx < INPUT_CAPTURE_MAX_KEYS) {
    return found_idx;
  }

  for (sz slot_idx = 1; slot_idx < INPUT_CAPTURE_MAX_KEYS; slot_idx += 1) {
    if (!input_capture_used[slot_idx]) {
      input_capture_keys[slot_idx] = key;
      input_capture_used[slot_idx] = 1;
      input_capture_slot_epoch[slot_idx] = input_capture_next_epoch(input_capture_slot_epoch[slot_idx]);
      return slot_idx;
    }
  }

  return INPUT_CAPTURE_MAX_KEYS;
}

func b32 input_capture_acquire_key(input_key key) {
  return input_capture_get_slot(key) < INPUT_CAPTURE_MAX_KEYS;
}

func void input_capture_release_key(input_key key) {
  input_capture_ensure_default_slot();

  if (key == INPUT_KEY_DEFAULT) {
    return;
  }

  sz slot_idx = input_capture_find_slot(key);
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return;
  }

  input_capture_keys[slot_idx] = 0;
  input_capture_used[slot_idx] = 0;
}

func void input_capture_release_all_keys(void) {
  input_capture_ensure_default_slot();

  for (sz slot_idx = 1; slot_idx < INPUT_CAPTURE_MAX_KEYS; slot_idx += 1) {
    input_capture_keys[slot_idx] = 0;
    input_capture_used[slot_idx] = 0;
  }
}

func u32 input_capture_get_slot_epoch(sz slot_idx) {
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  return input_capture_slot_epoch[slot_idx];
}

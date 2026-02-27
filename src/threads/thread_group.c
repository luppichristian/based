// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_group.h"
#include "../sdl3_include.h"

// SDL thread wrapper — bridges thread_func into thread_group_func.
static i32 thread_group_wrapper(void* raw) {
  thread_group_slot* slot = (thread_group_slot*)raw;
  return slot->entry(slot->index, slot->arg);
}

// Shared creation path. base_name may be NULL for unnamed threads.
static thread_group create_impl(u32 count, thread_group_func entry, void* arg, const c8* base_name) {
  thread_group empty = {0};
  if (!count || !entry) {
    return empty;
  }

  thread_group group = {0};
  group.threads = (thread*)SDL_malloc((size_t)count * sizeof(thread));
  group.slots = (thread_group_slot*)SDL_malloc((size_t)count * sizeof(thread_group_slot));

  if (!group.threads || !group.slots) {
    SDL_free(group.threads);
    SDL_free(group.slots);
    return empty;
  }

  for (u32 i = 0; i < count; i++) {
    group.slots[i].entry = entry;
    group.slots[i].arg = arg;
    group.slots[i].index = i;

    if (base_name) {
      c8 name_buf[256];
      SDL_snprintf(name_buf, sizeof(name_buf), "%s[%u]", base_name, i);
      group.threads[i] = thread_create_named(thread_group_wrapper, &group.slots[i], name_buf);
    } else {
      group.threads[i] = thread_create(thread_group_wrapper, &group.slots[i]);
    }

    if (!group.threads[i]) {
      for (u32 j = 0; j < i; j++) {
        thread_detach(group.threads[j]);
      }
      SDL_free(group.threads);
      SDL_free(group.slots);
      return empty;
    }
  }

  group.count = count;
  return group;
}

func thread_group _thread_group_create(u32 count, thread_group_func entry, void* arg, callsite site) {
  (void)site;
  return create_impl(count, entry, arg, NULL);
}

func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    const c8* base_name,
    callsite site) {
  (void)site;
  return create_impl(count, entry, arg, base_name);
}

func void _thread_group_destroy(thread_group* group, callsite site) {
  (void)site;
  if (!group) {
    return;
  }
  SDL_free(group->threads);
  SDL_free(group->slots);
  group->threads = NULL;
  group->slots = NULL;
  group->count = 0;
}

func b32 thread_group_is_valid(thread_group* group) {
  return group && group->threads != NULL;
}

func u32 thread_group_get_count(thread_group* group) {
  return group ? group->count : 0;
}

func thread thread_group_get(thread_group* group, u32 index) {
  if (!group || index >= group->count) {
    return NULL;
  }
  return group->threads[index];
}

func b32 thread_group_join_all(thread_group* group, i32* out_exit_codes) {
  if (!group || !group->threads) {
    return 0;
  }
  b32 ok = 1;
  for (u32 i = 0; i < group->count; i++) {
    i32 code = 0;
    if (!thread_join(group->threads[i], &code)) {
      ok = 0;
    }
    if (out_exit_codes) {
      out_exit_codes[i] = code;
    }
    group->threads[i] = NULL;
  }
  return ok;
}

func void thread_group_detach_all(thread_group* group) {
  if (!group || !group->threads) {
    return;
  }
  for (u32 i = 0; i < group->count; i++) {
    thread_detach(group->threads[i]);
    group->threads[i] = NULL;
  }
}

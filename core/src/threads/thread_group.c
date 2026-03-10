// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_group.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// SDL thread wrapper that bridges thread_func into thread_group_func.
func i32 thread_group_wrapper(void* raw) {
  profile_func_begin;
  thread_group_slot* slot = (thread_group_slot*)raw;
  if (slot == NULL || slot->entry == NULL) {
    profile_func_end;
    return 0;
  }
  assert(slot->idx < U32_MAX);
  profile_func_end;
  return slot->entry(slot->idx, slot->arg);
}

func allocator thread_group_allocator_resolve(void) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    profile_func_end;
    return alloc;
  }
  profile_func_end;
  return global_get_allocator();
}

// Shared creation path. base_name may be NULL for unnamed threads.
func thread_group create_impl(u32 count, thread_group_func entry, void* arg, cstr8 base_name) {
  profile_func_begin;
  allocator alloc = thread_group_allocator_resolve();
  thread_group empty = {0};
  if (!count || !entry) {
    thread_log_error("Rejected thread group creation count=%u has_entry=%u", count, (u32)(entry != NULL));
    profile_func_end;
    return empty;
  }
  if (alloc.alloc_fn == NULL || alloc.dealloc_fn == NULL) {
    thread_log_error("Failed to resolve allocator for thread group count=%u", count);
    profile_func_end;
    return empty;
  }
  assert(alloc.alloc_fn != NULL);
  assert(alloc.dealloc_fn != NULL);

  allocator main_allocator = {0};
  ctx* context = thread_ctx_get();
  if (context != NULL) {
    main_allocator = context->main_allocator;
  }

  thread_group group = {0};
  group.threads = (thread*)allocator_alloc(alloc, (sz)count * size_of(thread));
  group.slots = (thread_group_slot*)allocator_alloc(alloc, (sz)count * size_of(thread_group_slot));

  if (!group.threads || !group.slots) {
    thread_log_error("Failed to allocate thread group storage count=%u", count);
    allocator_dealloc(alloc, group.threads, (sz)count * size_of(thread));
    allocator_dealloc(alloc, group.slots, (sz)count * size_of(thread_group_slot));
    profile_func_end;
    return empty;
  }

  for (u32 idx = 0; idx < count; idx += 1) {
    group.slots[idx].entry = entry;
    group.slots[idx].arg = arg;
    group.slots[idx].idx = idx;

    if (base_name != NULL) {
      str8_medium name_buf = {0};
      cstr8_format(name_buf, size_of(name_buf), "%s[%u]", base_name, idx);
      group.threads[idx] = thread_create_named(thread_group_wrapper, &group.slots[idx], name_buf, main_allocator);
    } else {
      group.threads[idx] = thread_create(thread_group_wrapper, &group.slots[idx], main_allocator);
    }

    if (!group.threads[idx]) {
      thread_log_error("Failed to create thread group worker idx=%u base_name=%s",
                       idx,
                       base_name != NULL ? base_name : "<null>");
      for (u32 join_idx = 0; join_idx < idx; join_idx += 1) {
        thread_detach(group.threads[join_idx]);
      }
      allocator_dealloc(alloc, group.threads, (sz)count * size_of(thread));
      allocator_dealloc(alloc, group.slots, (sz)count * size_of(thread_group_slot));
      profile_func_end;
      return empty;
    }
  }

  group.count = count;
  thread_log_info("Created thread group count=%u base_name=%s", count, base_name != NULL ? base_name : "<null>");
  profile_func_end;
  return group;
}

func thread_group _thread_group_create(u32 count, thread_group_func entry, void* arg, callsite site) {
  profile_func_begin;
  (void)site;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = NULL,
                                                 });
  (void)msg_post(&lifecycle_msg);
  profile_func_end;
  return create_impl(count, entry, arg, NULL);
}

func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    cstr8 base_name,
    callsite site) {
  profile_func_begin;
  (void)site;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = NULL,
                                                 });
  (void)msg_post(&lifecycle_msg);
  profile_func_end;
  return create_impl(count, entry, arg, base_name);
}

func void _thread_group_destroy(thread_group* group, callsite site) {
  profile_func_begin;
  allocator alloc = thread_group_allocator_resolve();
  (void)site;
  if (!group) {
    thread_log_warn("Skipping thread group destroy for NULL group");
    profile_func_end;
    return;
  }
  if (alloc.dealloc_fn == NULL) {
    thread_log_error("Failed to resolve deallocator for thread group group=%p", (void*)group);
    profile_func_end;
    return;
  }
  assert(alloc.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = group,
                                                 });
  (void)msg_post(&lifecycle_msg);

  allocator_dealloc(alloc, group->threads, (sz)group->count * size_of(thread));
  allocator_dealloc(alloc, group->slots, (sz)group->count * size_of(thread_group_slot));
  group->threads = NULL;
  group->slots = NULL;
  group->count = 0;
  thread_log_info("Destroyed thread group group=%p", (void*)group);
  profile_func_end;
}

func b32 thread_group_is_valid(thread_group* group) {
  return group && group->threads != NULL;
}

func u32 thread_group_get_count(thread_group* group) {
  return group ? group->count : 0;
}

func thread thread_group_get(thread_group* group, u32 idx) {
  profile_func_begin;
  if (!group || idx >= group->count) {
    profile_func_end;
    return NULL;
  }
  profile_func_end;
  return group->threads[idx];
}

func b32 thread_group_join_all(thread_group* group, i32* out_exit_codes) {
  profile_func_begin;
  if (!group || !group->threads) {
    thread_log_error("Rejected thread group join for invalid group group=%p", (void*)group);
    profile_func_end;
    return false;
  }
  assert(group->count > 0);

  b32 success = true;
  for (u32 idx = 0; idx < group->count; idx += 1) {
    i32 exit_code = 0;
    if (!thread_join(group->threads[idx], &exit_code)) {
      thread_log_error("Failed to join thread group worker idx=%u group=%p", idx, (void*)group);
      success = 0;
    }
    if (out_exit_codes) {
      out_exit_codes[idx] = exit_code;
    }
    group->threads[idx] = NULL;
  }
  thread_log_info("Joined thread group count=%u success=%u", group->count, (u32)success);
  profile_func_end;
  return success;
}

func void thread_group_detach_all(thread_group* group) {
  profile_func_begin;
  if (!group || !group->threads) {
    thread_log_warn("Skipping thread group detach for invalid group group=%p", (void*)group);
    profile_func_end;
    return;
  }
  for (u32 idx = 0; idx < group->count; idx += 1) {
    thread_detach(group->threads[idx]);
    group->threads[idx] = NULL;
  }
  thread_log_info("Detached thread group count=%u", group->count);
  profile_func_end;
}

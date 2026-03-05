// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "msg_def.h"

// =========================================================================
// Input Messages
// =========================================================================

// Callback invoked when a message is posted through msg_post.
// Return 1 to allow posting, 0 to cancel.
typedef b32 (*msg_post_callback)(const msg* src, void* user_data);

// Processes backend events and refreshes the internal queue.
func void msg_pump(void);

// Pops the next queued message into out_msg. Returns 1 on success, 0 otherwise.
func b32 msg_poll(msg* out_msg);

// Waits indefinitely for the next message and writes it to out_msg.
func b32 msg_wait(msg* out_msg);

// Waits up to timeout_ms for the next message and writes it to out_msg.
func b32 msg_wait_timeout(msg* out_msg, i32 timeout_ms);

// Posts src into the backend queue. Returns 1 on success, 0 otherwise.
func b32 _msg_post(const msg* src, callsite site);
#define msg_post(src) _msg_post((src), CALLSITE_HERE)

// Reserves count consecutive custom event types and returns the first type value.
func u32 msg_register_custom_range(sz count);

// Registers a callback invoked before backend queue insertion.
// If the callback returns 0, posting is canceled.
func void msg_set_post_callback(msg_post_callback callback, void* user_data);

// Posts one object lifecycle message.
func b32 _msg_post_object_event(
    msg_object_event_kind event_kind,
    msg_object_type object_type,
    void* object_ptr,
    callsite site);
#define msg_post_object_event(event_kind, object_type, object_ptr) \
  _msg_post_object_event((event_kind), (object_type), (object_ptr), CALLSITE_HERE)

// Posts one thread-context lifecycle message.
func b32 _msg_post_thread_ctx_event(
    msg_thread_ctx_event_kind event_kind,
    void* ctx_ptr,
    u64 thread_id,
    callsite site);
#define msg_post_thread_ctx_event(event_kind, ctx_ptr, thread_id) \
  _msg_post_thread_ctx_event((event_kind), (ctx_ptr), (thread_id), CALLSITE_HERE)

// Converts a backend-native event object into a msg. Returns 1 on success, 0 otherwise.
func b32 msg_from_native(const void* native_event, msg* out_msg);

// Converts src into a backend-native event object. Returns 1 on success, 0 otherwise.
func b32 msg_to_native(const msg* src, void* native_event);

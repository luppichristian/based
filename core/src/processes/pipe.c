// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/pipe.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func pipe pipe_stdin(process prc) {
  profile_func_begin;
  if (!prc) {
    profile_func_end;
    return NULL;
  }
  assert(prc != NULL);

  pipe pip = (pipe)SDL_GetProcessInput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    (void)msg_post(&lifecycle_msg);
  }
  profile_func_end;
  return pip;
}

func pipe pipe_stdout(process prc) {
  profile_func_begin;
  if (!prc) {
    profile_func_end;
    return NULL;
  }
  assert(prc != NULL);

  pipe pip = (pipe)SDL_GetProcessOutput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    (void)msg_post(&lifecycle_msg);
  }
  profile_func_end;
  return pip;
}

func pipe pipe_stderr(process prc) {
  profile_func_begin;
  if (!prc) {
    profile_func_end;
    return NULL;
  }
  assert(prc != NULL);

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    profile_func_end;
    return NULL;
  }

  pipe pip = (pipe)SDL_GetPointerProperty(props, SDL_PROP_PROCESS_STDERR_POINTER, NULL);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    (void)msg_post(&lifecycle_msg);
  }
  profile_func_end;
  return pip;
}

func b32 pipe_is_valid(pipe pip) {
  profile_func_begin;
  profile_func_end;
  return pip != NULL;
}

func sz pipe_read(pipe pip, void* ptr, sz size) {
  profile_func_begin;
  if (!pip || !ptr || !size) {
    profile_func_end;
    return 0;
  }
  assert(size > 0);

  profile_func_end;
  return (sz)SDL_ReadIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func sz pipe_write(pipe pip, const void* ptr, sz size) {
  profile_func_begin;
  if (!pip || !ptr || !size) {
    profile_func_end;
    return 0;
  }
  assert(size > 0);

  profile_func_end;
  return (sz)SDL_WriteIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func sz pipe_read_nonblocking(pipe pip, void* ptr, sz size) {
  profile_func_begin;
  sz result = pipe_read(pip, ptr, size);
  profile_func_end;
  return result;
}

func sz pipe_write_nonblocking(pipe pip, const void* ptr, sz size) {
  profile_func_begin;
  sz result = pipe_write(pip, ptr, size);
  profile_func_end;
  return result;
}

func b32 pipe_poll_readable(pipe pip, i32 timeout_ms) {
  profile_func_begin;
  if (!pipe_is_valid(pip) || timeout_ms < 0) {
    profile_func_end;
    return 0;
  }
  if (timeout_ms > 0) {
    SDL_Delay((u32)timeout_ms);
  }
  profile_func_end;
  return 1;
}

func b32 pipe_flush(pipe pip) {
  profile_func_begin;
  if (!pip) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return SDL_FlushIO((SDL_IOStream*)pip);
}

func void pipe_close(pipe pip) {
  profile_func_begin;
  if (!pip) {
    profile_func_end;
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                     .object_ptr = pip,
                                                 });
  (void)msg_post(&lifecycle_msg);

  thread_log_trace("pipe_close: pipe=%p", pip);
  SDL_CloseIO((SDL_IOStream*)pip);
  profile_func_end;
}

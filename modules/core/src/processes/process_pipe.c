// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process_pipe.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func process_pipe _process_pipe_stdin(process prc, callsite site) {
  profile_func_begin;
  if (!prc) {
    profile_func_end;
    return NULL;
  }
  assert(prc != NULL);

  process_pipe pip = (process_pipe)SDL_GetProcessInput((SDL_Process*)prc);
  if (pip != NULL) {
    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
        .object_ptr = pip,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      thread_log_trace("Process pipe stdin acquisition was suspended process=%p pipe=%p", prc, pip);
      profile_func_end;
      return NULL;
    }
  }
  profile_func_end;
  return pip;
}

func process_pipe _process_pipe_stdout(process prc, callsite site) {
  profile_func_begin;
  if (!prc) {
    profile_func_end;
    return NULL;
  }
  assert(prc != NULL);

  process_pipe pip = (process_pipe)SDL_GetProcessOutput((SDL_Process*)prc);
  if (pip != NULL) {
    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
        .object_ptr = pip,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      thread_log_trace("Process pipe stdout acquisition was suspended process=%p pipe=%p", prc, pip);
      profile_func_end;
      return NULL;
    }
  }
  profile_func_end;
  return pip;
}

func process_pipe _process_pipe_stderr(process prc, callsite site) {
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

  process_pipe pip = (process_pipe)SDL_GetPointerProperty(props, SDL_PROP_PROCESS_STDERR_POINTER, NULL);
  if (pip != NULL) {
    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
        .object_ptr = pip,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      thread_log_trace("Process pipe stderr acquisition was suspended process=%p pipe=%p", prc, pip);
      profile_func_end;
      return NULL;
    }
  }
  profile_func_end;
  return pip;
}

func b32 process_pipe_is_valid(process_pipe pip) {
  return pip != NULL;
}

func sz process_pipe_read(process_pipe pip, void* ptr, sz size) {
  profile_func_begin;
  if (!pip || !ptr || !size) {
    thread_log_error("Rejected process pipe read pipe=%p size=%zu", pip, (size_t)size);
    profile_func_end;
    return 0;
  }
  assert(size > 0);

  sz result = (sz)SDL_ReadIO((SDL_IOStream*)pip, ptr, (size_t)size);
  if (result == 0 && size > 0) {
    thread_log_warn("Process pipe read produced no data pipe=%p error=%s", pip, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func sz process_pipe_write(process_pipe pip, const void* ptr, sz size) {
  profile_func_begin;
  if (!pip || !ptr || !size) {
    thread_log_error("Rejected process pipe write pipe=%p size=%zu", pip, (size_t)size);
    profile_func_end;
    return 0;
  }
  assert(size > 0);

  sz result = (sz)SDL_WriteIO((SDL_IOStream*)pip, ptr, (size_t)size);
  if (result != size) {
    thread_log_error("Failed to write full process pipe pipe=%p expected=%zu actual=%zu error=%s",
                     pip,
                     (size_t)size,
                     (size_t)result,
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}

func sz process_pipe_read_nonblocking(process_pipe pip, void* ptr, sz size) {
  profile_func_begin;
  sz result = process_pipe_read(pip, ptr, size);
  profile_func_end;
  return result;
}

func sz process_pipe_write_nonblocking(process_pipe pip, const void* ptr, sz size) {
  profile_func_begin;
  sz result = process_pipe_write(pip, ptr, size);
  profile_func_end;
  return result;
}

func b32 process_pipe_poll_readable(process_pipe pip, i32 timeout_ms) {
  profile_func_begin;
  if (!process_pipe_is_valid(pip) || timeout_ms < 0) {
    thread_log_error("Rejected process pipe poll pipe=%p timeout_ms=%d", pip, timeout_ms);
    profile_func_end;
    return false;
  }
  if (timeout_ms > 0) {
    SDL_Delay((u32)timeout_ms);
  }
  profile_func_end;
  return true;
}

func b32 process_pipe_flush(process_pipe pip) {
  if (!pip) {
    thread_log_error("Cannot flush invalid process pipe");
    return false;
  }

  b32 success = SDL_FlushIO((SDL_IOStream*)pip);
  if (!success) {
    thread_log_error("Failed to flush process pipe pipe=%p error=%s", pip, SDL_GetError());
  }
  return success;
}

func void _process_pipe_close(process_pipe pip, callsite site) {
  profile_func_begin;
  if (!pip) {
    profile_func_end;
    return;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
      .object_ptr = pip,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Process pipe close was suspended pipe=%p", pip);
    profile_func_end;
    return;
  }

  thread_log_trace("Closed process pipe pipe=%p", pip);
  SDL_CloseIO((SDL_IOStream*)pip);
  profile_func_end;
}

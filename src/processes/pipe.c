// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/pipe.h"
#include "input/msg.h"
#include "../sdl3_include.h"

func pipe pipe_stdin(process prc) {
  if (!prc) {
    return NULL;
  }

  pipe pip = (pipe)SDL_GetProcessInput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_PIPE;
    lifecycle_msg.object_lifecycle.object_ptr = pip;
    if (!msg_post(&lifecycle_msg)) {
      return NULL;
    }
  }
  return pip;
}

func pipe pipe_stdout(process prc) {
  if (!prc) {
    return NULL;
  }

  pipe pip = (pipe)SDL_GetProcessOutput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_PIPE;
    lifecycle_msg.object_lifecycle.object_ptr = pip;
    if (!msg_post(&lifecycle_msg)) {
      return NULL;
    }
  }
  return pip;
}

func pipe pipe_stderr(process prc) {
  if (!prc) {
    return NULL;
  }

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    return NULL;
  }

  pipe pip = (pipe)SDL_GetPointerProperty(props, SDL_PROP_PROCESS_STDERR_POINTER, NULL);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_PIPE;
    lifecycle_msg.object_lifecycle.object_ptr = pip;
    if (!msg_post(&lifecycle_msg)) {
      return NULL;
    }
  }
  return pip;
}

func b32 pipe_is_valid(pipe pip) {
  return pip != NULL;
}

func sz pipe_read(pipe pip, void* ptr, sz size) {
  if (!pip || !ptr || !size) {
    return 0;
  }

  return (sz)SDL_ReadIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func sz pipe_write(pipe pip, const void* ptr, sz size) {
  if (!pip || !ptr || !size) {
    return 0;
  }

  return (sz)SDL_WriteIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func b32 pipe_flush(pipe pip) {
  if (!pip) {
    return 0;
  }

  return SDL_FlushIO((SDL_IOStream*)pip);
}

func void pipe_close(pipe pip) {
  if (!pip) {
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_PIPE;
  lifecycle_msg.object_lifecycle.object_ptr = pip;
  if (!msg_post(&lifecycle_msg)) {
    return;
  }

  SDL_CloseIO((SDL_IOStream*)pip);
}

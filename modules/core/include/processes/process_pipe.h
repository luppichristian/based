// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "process.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Process Pipe
// =========================================================================

// Opaque handle to a process-owned stdio stream pipe.
typedef void* process_pipe;

// Returns the process stdin pipe for a spawned process when pipe_stdin was enabled.
// Returns NULL when stdin is not available.
func process_pipe _process_pipe_stdin(process prc, callsite site);

// Returns the process stdout pipe for a spawned process when pipe_stdout was enabled.
// Returns NULL when stdout is not available.
func process_pipe _process_pipe_stdout(process prc, callsite site);

// Returns the process stderr pipe for a spawned process when pipe_stderr was enabled.
// Returns NULL when stderr is not available.
func process_pipe _process_pipe_stderr(process prc, callsite site);

// Returns true if pip is a valid process-pipe handle.
func b32 process_pipe_is_valid(process_pipe pip);

// Reads up to size bytes from a process pipe.
// Returns the number of bytes read.
func sz process_pipe_read(process_pipe pip, void* ptr, sz size);

// Writes up to size bytes to a process pipe.
// Returns the number of bytes written.
func sz process_pipe_write(process_pipe pip, const void* ptr, sz size);
// Non-blocking I/O and poll helpers.
func sz process_pipe_read_nonblocking(process_pipe pip, void* ptr, sz size);
func sz process_pipe_write_nonblocking(process_pipe pip, const void* ptr, sz size);
func b32 process_pipe_poll_readable(process_pipe pip, i32 timeout_ms);

// Flushes buffered data for a writable process pipe.
func b32 process_pipe_flush(process_pipe pip);

// Closes a process pipe.
// Closing stdin is the portable way to signal EOF to the child process.
func void _process_pipe_close(process_pipe pip, callsite site);

#define process_pipe_stdin(prc) \
  _process_pipe_stdin(prc, CALLSITE_HERE)
#define process_pipe_stdout(prc) \
  _process_pipe_stdout(prc, CALLSITE_HERE)
#define process_pipe_stderr(prc) \
  _process_pipe_stderr(prc, CALLSITE_HERE)
#define process_pipe_close(pip) \
  _process_pipe_close(pip, CALLSITE_HERE)

// =========================================================================
c_end;
// =========================================================================

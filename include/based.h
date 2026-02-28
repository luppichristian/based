// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Include basic module.
#include "basic/assert.h"
#include "basic/codespace.h"
#include "basic/env_defines.h"
#include "basic/keyword_defines.h"
#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// Include container modules.
#include "containers/doubly_list.h"
#include "containers/singly_list.h"
#include "containers/stack_list.h"

// Include memory modules.
#include "memory/allocator.h"
#include "memory/arena.h"
#include "memory/buffer.h"
#include "memory/heap.h"
#include "memory/pool.h"
#include "memory/ring.h"
#include "memory/scratch.h"
#include "memory/vmem.h"

// Include threading modules.
#include "threads/atomics.h"
#include "threads/condvar.h"
#include "threads/mutex.h"
#include "threads/rwlock.h"
#include "threads/semaphore.h"
#include "threads/spinlock.h"
#include "threads/thread.h"
#include "threads/thread_current.h"
#include "threads/thread_group.h"
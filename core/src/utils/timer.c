// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/timer.h"
#include "basic/assert.h"
#include "basic/profiler.h"

func void timer_clear(f32* timer) {
  profile_func_begin;
  if (timer == NULL) {
    profile_func_end;
    return;
  }
  assert(timer != NULL);
  *timer = 0.0f;
  profile_func_end;
}

func void timer_bump(f32* timer) {
  profile_func_begin;
  if (timer == NULL) {
    profile_func_end;
    return;
  }
  assert(timer != NULL);
  *timer = VERY_HIGH_TIMER_VALUE;
  profile_func_end;
}

func void timer_increment(f32* timer, f32 dt, f32 scale) {
  profile_func_begin;
  if (timer == NULL) {
    profile_func_end;
    return;
  }
  assert(timer != NULL);
  *timer += dt * scale;
  profile_func_end;
}

func b32 timer_consume(f32* timer, f32 rate) {
  profile_func_begin;
  if (timer == NULL || rate <= 0.0f || *timer < rate) {
    profile_func_end;
    return 0;
  }
  assert(timer != NULL);

  *timer -= rate;
  profile_func_end;
  return 1;
}

func b32 timer_consume_once(f32* timer, f32 rate) {
  profile_func_begin;
  if (timer == NULL || rate <= 0.0f || *timer < rate) {
    profile_func_end;
    return 0;
  }
  assert(timer != NULL);

  timer_clear(timer);
  profile_func_end;
  return 1;
}

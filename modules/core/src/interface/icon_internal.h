// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../../include/interface/icon.h"
#include "../sdl3_include.h"

typedef struct icon_state icon_state;

func icon_state* icon_internal_resolve(icon icon_id);
func SDL_Surface* icon_internal_get_surface(icon icon_id);
func b32 icon_internal_get_system_cursor(icon icon_id, SDL_SystemCursor* out_system_cursor);

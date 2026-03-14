// MIT License
// Copyright (c) 2026 Christian Luppi

#define SDL_COMPILE_TIME_ASSERT(name, x) _Static_assert(x, #name)

#define SDL_MAIN_HANDLED 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

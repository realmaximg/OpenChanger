#pragma once

#include "sdk/macros.h"

class SDL {
public:
  SDK_CLASS(SDL);

  using MainThreadCallback = void (*)(void *);

  SDK_EXPORT_FUNCTION(SDL3, bool(MainThreadCallback, void *, bool),
                      RunOnMainThread, "SDL_RunOnMainThread");
  SDK_EXPORT_FUNCTION(SDL3, bool(), IsMainThread, "SDL_IsMainThread");
};

#pragma once

#include "sdk/macros.h"
#include "sdk/resourcesystem/ResourceBinding_t.h"

#include <cstdint>

#define PtFindResidentBinding                                                  \
  "49 8B 10 4C 8B 00 48 8B 01 FF 90 * * * * 48 8B D8"

class CResourceSystem {
public:
  SDK_INTERFACE(ResourceSystem, CResourceSystem, "ResourceSystem013");

  PATTERN_VIRTUAL(NetworkSystem,
                  ResourceBinding_t *(std::uint64_t, std::uint64_t),
                  FindResidentBinding, PtFindResidentBinding);
};

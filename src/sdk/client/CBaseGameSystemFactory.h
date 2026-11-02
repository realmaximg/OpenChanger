#pragma once

#include "sdk/macros.h"

#include <string_view>

#define Ptsm_pFirst                                                            \
  "48 8D 05 ? ? ? ? 48 89 51 10 48 89 01 48 8B 05 ? ? ? ? 48 89 41 08 48 8B "  \
  "C1 48 89 0D * * * * C3"
#define Ptm_pNext                                                              \
  "48 8D 05 ? ? ? ? 48 89 51 10 48 89 01 48 8B 05 ? ? ? ? 48 89 41 * 48 8B "   \
  "C1 48 89 0D ? ? ? ? C3"
#define Ptm_pName                                                              \
  "48 8D 05 ? ? ? ? 48 89 51 * 48 89 01 48 8B 05 ? ? ? ? 48 89 41 08 48 8B "   \
  "C1 48 89 0D ? ? ? ? C3"
#define PtGetStaticGameSystem                                                  \
  "48 8B CE FF 50 40 84 C0 0F 85 ? ? ? ? 48 8B 06 48 8B CE FF 50 *"

class CBaseGameSystemFactory {
public:
  SDK_CLASS(CBaseGameSystemFactory);

  PATTERN_GLOBAL(Client, CBaseGameSystemFactory *, sm_pFirst, Ptsm_pFirst);
  PATTERN_FIELD(Client, CBaseGameSystemFactory *, m_pNext, Ptm_pNext);
  PATTERN_FIELD(Client, const char *, m_pName, Ptm_pName);
  PATTERN_VIRTUAL(Client, void *(), GetStaticGameSystem, PtGetStaticGameSystem);

  static void *find(std::string_view Name) {
    for (CBaseGameSystemFactory *Factory = sm_pFirst(); Factory != nullptr;
         Factory = Factory->m_pNext()) {
      if (std::string_view(Factory->m_pName()) == Name) {
        return Factory->GetStaticGameSystem();
      }
    }
    return nullptr;
  }
};

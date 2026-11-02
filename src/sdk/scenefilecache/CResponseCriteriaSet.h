#pragma once

#include "sdk/macros.h"
#include "sdk/tier1/CUtlSymbolTable.h"

#define Ptsm_CriteriaSymbols                                                   \
  "4C 8B C2 48 8B D9 48 8B D1 45 33 C9 48 8B 0D * * * * FF 15 ? ? ? ? 48 8B "  \
  "C3 48 83 C4 20 5B C3"

class CResponseCriteriaSet {
public:
  SDK_CLASS(CResponseCriteriaSet);

  PATTERN_GLOBAL(SceneFileCache, CUtlSymbolTable *, sm_CriteriaSymbols,
                 Ptsm_CriteriaSymbols);
};

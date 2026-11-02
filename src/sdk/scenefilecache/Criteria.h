#pragma once

#include "sdk/macros.h"
#include "sdk/scenefilecache/CResponseCriteriaSet.h"
#include "sdk/tier1/CUtlSymbolTable.h"

#include <array>
#include <cstddef>

struct Criteria {
  CUtlSymbol nameSym;
  PAD(0x6);
  const char *value;
  PAD(0x10);

  [[nodiscard]] const char *name() const {
    return CUtlSymbolTable::String(CResponseCriteriaSet::sm_CriteriaSymbols(),
                                   &nameSym);
  }
};

static_assert(sizeof(Criteria) == 0x20);
static_assert(offsetof(Criteria, nameSym) == 0x0);
static_assert(offsetof(Criteria, value) == 0x8);

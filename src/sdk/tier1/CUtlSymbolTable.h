#pragma once

#include "sdk/macros.h"

#include <cstdint>

struct CUtlSymbol {
  std::uint16_t m_Id;
};

static_assert(sizeof(CUtlSymbol) == 0x2);

class CUtlSymbolTable {
public:
  SDK_CLASS(CUtlSymbolTable);

  SDK_EXPORT_FUNCTION(Tier0,
                      const char *(const CUtlSymbolTable *, const CUtlSymbol *),
                      String,
                      "?String@CUtlSymbolTable@@QEBAPEBDVCUtlSymbol@@@Z");
};

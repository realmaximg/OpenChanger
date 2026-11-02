#pragma once

#include "sdk/client/CCSPlayerController.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlStringToken.h"

#include <cstdint>

#define PtGetName                                                              \
  "48 8B 06 48 8B CE FF 50 * 48 8B D0 48 8D 0D ? ? ? ? FF 15 ? ? ? ? 49 8B "   \
  "04 24 48 8B D6 49 8B CC FF 50 50"
#define PtGetString                                                            \
  "48 8B 02 48 8D 0D ? ? ? ? 4C 8B F2 41 B8 FA DA 03 3E BA 02 00 00 00 48 8B " \
  "58 *"
#define PtGetPlayerController                                                  \
  "49 8B 4D 00 BA 04 00 00 00 41 B8 13 F2 E4 ED 48 8B F0 48 8B 99 * * * *"
#define PtSetInt                                                               \
  "48 8B 07 48 8D 0D ? ? ? ? BA 06 00 00 00 41 B8 96 8E 4F FA 48 8B 98 * * * " \
  "*"
#define PtSetString                                                            \
  "49 8B 45 00 48 8B B8 * * * * 48 85 DB 75 07 48 8D 1D ? ? ? ? 33 D2 48 8D "  \
  "0D ? ? ? ? 41 B8 A0 20 76 FF"

struct GameEventKeySymbol_t {
  explicit GameEventKeySymbol_t(const char *Name) : m_pszName(Name) {
    CUtlStringToken Token;
    CUtlStringToken::Make(&Token, Name);
    m_nHashCode = Token.m_nHashCode;
  }

  std::uint32_t m_nHashCode = 0;
  std::int32_t m_nIndex = -1;
  const char *m_pszName;
};

static_assert(sizeof(GameEventKeySymbol_t) == 0x10);

class IGameEvent {
public:
  SDK_CLASS(IGameEvent);

  PATTERN_VIRTUAL(Client, const char *(), GetName, PtGetName);
  PATTERN_VIRTUAL(Client,
                  const char *(const GameEventKeySymbol_t *, const char *),
                  GetString, PtGetString);
  PATTERN_VIRTUAL(Client, CBasePlayerController *(const GameEventKeySymbol_t *),
                  GetPlayerController, PtGetPlayerController);
  PATTERN_VIRTUAL(Client, void(const GameEventKeySymbol_t *, std::int32_t),
                  SetInt, PtSetInt);
  PATTERN_VIRTUAL(Client, void(const GameEventKeySymbol_t *, const char *),
                  SetString, PtSetString);

  bool isLocalPlayer(const char *Key) {
    const CCSPlayerController *const My = CCSPlayerController::getLocal();
    const GameEventKeySymbol_t Symbol(Key);
    return My != nullptr && GetPlayerController(&Symbol) == My;
  }
};

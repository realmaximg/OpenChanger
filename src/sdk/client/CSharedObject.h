#pragma once

#include "game/find.h"
#include "game/module.h"
#include "sdk/client/NativeString.h"
#include "sdk/macros.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#define PtBAddToMessage                                                        \
  "4C 8B DC 49 89 5B 08 49 89 6B 10 49 89 73 18 57 48 83 EC 60 49 C7 43 C0 ? " \
  "? ? ? 48 8D 05 ? ? ? ? 49 89 43 B8 41 0F B6 F8 48 8D 05 ? ? ? ? 48 8B EA "  \
  "49 89 43 C8 4D 8D 43 D8 0F 10 44 24 20 48 8D 15 ? ? ? ? 48 8B F1 F2 0F 10 " \
  "4C 24 30 48 8D 0D * * * *"
#define PtGetTypeID                                                            \
  "41 83 F9 04 0F 85 ? ? ? ? 49 8B 00 49 8B C8 48 89 74 24 48 FF 50 *"

namespace GCSDK {

inline constexpr std::int32_t k_EEconTypeItem = 1;
inline constexpr std::int32_t k_EEconTypeEquipSlot = 3;

class CSharedObject {
  static std::optional<std::uint32_t> findBAddToMessage() {
    return oc::findSlotNamed(oc::GameModule::Client, "CEconItem",
                             PtBAddToMessage,
                             "CEconItem::BAddToMessage::std::string");
  }

public:
  SDK_CLASS(CSharedObject);

  PATTERN_VIRTUAL(Client, std::int32_t(), GetTypeID, PtGetTypeID);

  VIRTUAL_FIND(bool(NativeString *), BAddToMessage, findBAddToMessage);

  std::optional<std::string> bytes() {
    NativeString Bytes;
    if (!BAddToMessage(&Bytes)) {
      return std::nullopt;
    }
    return std::string(Bytes.view());
  }
};

} // namespace GCSDK

#include "game/pattern.h"

#include "game/module.h"

#include <libhat/scanner.hpp>
#include <libhat/signature.hpp>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace oc {

namespace {

struct Stars {
  std::size_t Position = 0;
  std::size_t Size = 0;
};

std::vector<std::string_view> tokensOf(std::string_view Pattern) {
  std::vector<std::string_view> Tokens;
  for (const auto Token : std::views::split(Pattern, ' ')) {
    if (!Token.empty()) {
      Tokens.emplace_back(Token.begin(), Token.end());
    }
  }
  return Tokens;
}

std::optional<Stars> starsOf(std::string_view Pattern) {
  const auto Tokens = tokensOf(Pattern);
  const auto First = std::ranges::find(Tokens, "*");
  if (First == Tokens.end()) {
    return std::nullopt;
  }
  const auto End = std::find_if(
      First, Tokens.end(), [](std::string_view Token) { return Token != "*"; });
  if (std::find(End, Tokens.end(), "*") != Tokens.end()) {
    return std::nullopt;
  }
  const Stars Result{
      .Position = static_cast<std::size_t>(First - Tokens.begin()),
      .Size = static_cast<std::size_t>(End - First),
  };
  if (Result.Size != 1 && Result.Size != 4) {
    return std::nullopt;
  }
  return Result;
}

auto signatureOf(std::string_view Pattern) {
  std::string Wildcards(Pattern);
  std::ranges::replace(Wildcards, '*', '?');
  return hat::parse_signature(Wildcards);
}

std::span<const std::byte> matchAt(GameModule Module, std::uintptr_t Address,
                                   std::string_view Pattern) {
  const auto Parsed = signatureOf(Pattern);
  const std::span<const std::byte> Text = sectionOf(Module, ".text");
  const std::uintptr_t Offset =
      Address - std::bit_cast<std::uintptr_t>(Text.data());
  if (!Parsed.has_value() || Offset > Text.size() ||
      Parsed.value().size() > Text.size() - Offset) {
    return {};
  }
  const auto Bytes = Text.subspan(Offset, Parsed.value().size());
  if (!std::ranges::equal(Bytes, Parsed.value(),
                          [](std::byte Byte, hat::signature_element Element) {
                            return Element == Byte;
                          })) {
    return {};
  }
  return Bytes;
}

std::span<const std::byte> valueAt(GameModule Module, std::uintptr_t Address,
                                   std::string_view Pattern) {
  const auto At = starsOf(Pattern);
  const auto Bytes = matchAt(Module, Address, Pattern);
  if (!At || Bytes.empty()) {
    return {};
  }
  return Bytes.subspan(At->Position, At->Size);
}

} // namespace

std::optional<std::uintptr_t> findPattern(GameModule Module,
                                          std::string_view Pattern) {
  const auto Parsed = signatureOf(Pattern);
  if (!Parsed.has_value()) {
    return std::nullopt;
  }
  const std::span<const std::byte> Text = sectionOf(Module, ".text");
  const auto *const End = Text.data() + Text.size();
  const auto *const First =
      hat::find_pattern(Text.data(), End, Parsed.value()).get();
  if (First == nullptr ||
      hat::find_pattern(First + 1, End, Parsed.value()).has_result()) {
    return std::nullopt;
  }
  return std::bit_cast<std::uintptr_t>(First);
}

std::optional<std::uint32_t> patternValueAt(GameModule Module,
                                            std::uintptr_t Address,
                                            std::string_view Pattern) {
  const auto Value = valueAt(Module, Address, Pattern);
  if (Value.empty()) {
    return std::nullopt;
  }
  if (Value.size() == 1) {
    return static_cast<std::uint32_t>(std::to_integer<std::int8_t>(Value[0]));
  }
  std::uint32_t Wide = 0;
  std::memcpy(&Wide, Value.data(), sizeof(Wide));
  return Wide;
}

std::optional<std::uintptr_t> patternTargetAt(GameModule Module,
                                              std::uintptr_t Address,
                                              std::string_view Pattern) {
  const auto Value = valueAt(Module, Address, Pattern);
  if (Value.size() != 4) {
    return std::nullopt;
  }
  std::int32_t Displacement = 0;
  std::memcpy(&Displacement, Value.data(), sizeof(Displacement));
  return std::bit_cast<std::uintptr_t>(Value.data() + Value.size()) +
         static_cast<std::uintptr_t>(Displacement);
}

} // namespace oc

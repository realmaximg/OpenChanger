#pragma once

#include <string>
#include <string_view>

namespace oc {

inline std::string lower(std::string_view Text) {
  std::string Result(Text);
  for (char &Letter : Result) {
    if (Letter >= 'A' && Letter <= 'Z') {
      Letter = static_cast<char>(Letter - 'A' + 'a');
    }
  }
  return Result;
}

} // namespace oc

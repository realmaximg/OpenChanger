#include "game/js.h"

#include <filesystem>
#include <fstream>
#include <ios>

int main(int Count, char *const *Arguments) {
  if (Count != 2) {
    return 2;
  }
  const std::filesystem::path Path(Arguments[1]);
  std::filesystem::create_directories(Path.parent_path());
  std::ofstream(Path, std::ios::binary) << oc::apiDeclarations();
  return 0;
}

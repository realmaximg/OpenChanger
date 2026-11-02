#pragma once

#include "sdk/filesystem/FileFindHandle_t.h"
#include "sdk/macros.h"

#include <format>
#include <string>
#include <string_view>
#include <vector>

#define PtFindFirstEx "4C 8B C6 48 8B 01 4C 0F 4F 44 24 48 FF 90 * * * *"
#define PtFindNext "48 8D 54 24 48 48 8B 01 FF 90 * * * * 48 89 44 24 60"
#define PtFindClose "48 8D 54 24 48 48 8B 01 FF 90 * * * * 45 33 C9"

class CFileSystem_Stdio {
public:
  SDK_INTERFACE(FileSystem, CFileSystem_Stdio, "VFileSystem017");

  PATTERN_VIRTUAL(Client,
                  const char *(const char *, const char *, FileFindHandle_t *),
                  FindFirstEx, PtFindFirstEx);
  PATTERN_VIRTUAL(Client, const char *(FileFindHandle_t *), FindNext,
                  PtFindNext);
  PATTERN_VIRTUAL(Client, void(FileFindHandle_t *), FindClose, PtFindClose);

  static constexpr const char *GamePath = "GAME";
  static constexpr std::string_view PaintPreviewFolder =
      "panorama/images/econ/default_generated/";
  static constexpr std::string_view PaintPreviewSuffix = "_light_png.vtex_c";

  std::vector<std::string> findFiles(const char *Mask, const char *PathId) {
    FileFindHandle_t Handle{};
    std::vector<std::string> Files;
    for (const char *File = FindFirstEx(Mask, PathId, &Handle); File != nullptr;
         File = FindNext(&Handle)) {
      Files.emplace_back(File);
    }
    FindClose(&Handle);
    return Files;
  }

  std::vector<std::string> findPaintPreviews() {
    const std::string Mask =
        std::format("{}*{}", PaintPreviewFolder, PaintPreviewSuffix);
    std::vector<std::string> Names;
    for (const std::string &File : findFiles(Mask.c_str(), GamePath)) {
      std::string_view Name =
          std::string_view(File).substr(File.rfind('/') + 1);
      if (Name.ends_with(PaintPreviewSuffix)) {
        Name.remove_suffix(PaintPreviewSuffix.size());
        Names.emplace_back(Name);
      }
    }
    return Names;
  }
};

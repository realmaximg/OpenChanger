#include "ui/ui.h"

#include "game/panorama.h"

#include <string_view>

#ifdef OPENCHANGER_DEV
#include "core/log.h"
#include "game/js.h"

#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>
#endif

namespace oc::ui {

namespace {

#ifdef OPENCHANGER_DEV

std::optional<std::string> readFile(const char *Path) {
  std::ifstream File(Path, std::ios::binary);
  if (!File) {
    OC_LOG("ui", "{} not found", Path);
    return std::nullopt;
  }
  return std::string(std::istreambuf_iterator<char>(File), {});
}

void log(const std::string &Text) { OC_LOG("js", "{}", Text); }

JsMethod<&log> Log{"dev", "log", "text"};

#else

constexpr char Script[] = {
#embed OC_UI_BUNDLE_JS suffix(, )
    0};
constexpr char Css[] = {
#embed OC_UI_BUNDLE_CSS suffix(, )
    0};

#endif

} // namespace

bool start() {
#ifdef OPENCHANGER_DEV
  const auto Script = readFile(OC_UI_DIST "/bundle.js");
  const auto Css = readFile(OC_UI_DIST "/bundle.css");
  if (!Script || !Css) {
    OC_LOG("ui", "no frontend build, the UI is off");
    return true;
  }
  return startPanorama(*Css, Script->c_str());
#else
  return startPanorama({Css, sizeof(Css) - 1}, Script);
#endif
}

void stop() { stopPanorama(); }

} // namespace oc::ui

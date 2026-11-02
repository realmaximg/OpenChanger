#include "game/panorama.h"

#include "core/log.h"
#include "game/js.h"
#include "sdk/client/client.h"
#include "sdk/panorama/CLayoutManager.h"
#include "sdk/panorama/CPanel2D.h"
#include "sdk/panorama/CUIEngine.h"
#include "sdk/panorama/CUIPanel.h"
#include "sdk/panorama/IPanoramaUIEngine.h"
#include "sdk/sdl3/SDL.h"
#include "sdk/v8/Context.h"
#include "sdk/v8/FunctionCallbackInfo.h"
#include "sdk/v8/FunctionTemplate.h"
#include "sdk/v8/HandleScope.h"
#include "sdk/v8/Isolate.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/String.h"
#include "sdk/v8/Value.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace oc {

namespace {

constexpr const char *FunctionName = "__openchanger";
constexpr const char *StyleUrl = "file://{resources}/styles/openchanger.css";
constexpr const char *StylePath = R"(panorama\styles\openchanger.css)";
constexpr std::uint64_t FirstLine = 1;
constexpr std::int32_t NotifyArguments = 2;

constexpr const char *StopScript = R"js((function () {
  if (globalThis.OpenChangerAPI) {
    OpenChangerAPI.root.DeleteAsync(0);
  }
  globalThis.OpenChangerAPI = undefined;
})();
)js";

bool Started = false;
std::uint32_t Starts = 0;
std::vector<std::int32_t> Subscribers;

CUIEngine *engine() { return IPanoramaUIEngine::get()->AccessUIEngine(); }

CUIPanel *mainMenu() {
  CPanel2D *const Menu = client::MainMenu();
  return Menu != nullptr ? Menu->UIPanel() : nullptr;
}

std::string rootId() { return std::format("OpenChanger{}", Starts); }

void run(CUIPanel *Menu, const char *Origin, const char *Source) {
  engine()->RunScript(Menu, Source, Origin, FirstLine);
}

std::string apiScript() {
  std::string Script = std::format(
      "(function () {{\n"
      "  const call = (method) => (...args) => {}(method, ...args);\n"
      "  const api = {{ root: $.CreatePanel('Panel', $.GetContextPanel(), "
      "'{}', {{ hittest: 'false', style: 'width: 100%; height: 100%;' }}) "
      "}};\n",
      FunctionName, rootId());
  const auto Methods = jsMethods();
  for (std::size_t Number = 0; Number < Methods.size(); ++Number) {
    Script +=
        std::format("  (api.{} ||= {{}}).{} = call({});\n",
                    Methods[Number]->Feature, Methods[Number]->Name, Number);
  }
  Script += "  globalThis.OpenChangerAPI = api;\n})();\n";
  return Script;
}

struct JsCallback {
  v8::Local<v8::Value> Function;
};

} // namespace

template <> struct JsType<JsCallback> {
  static std::string declare(JsDeclarations & /*Types*/) {
    return "(feature: string, event: string) => void";
  }
  static bool read(const JsCall & /*Call*/, v8::Local<v8::Value> Value,
                   JsCallback &Into) {
    if (!JsCall::isFunction(Value)) {
      return false;
    }
    Into.Function = Value;
    return true;
  }
};

namespace {

std::optional<std::int32_t> subscribe(JsCallback Callback) {
  CUIPanel *const Menu = mainMenu();
  if (Menu == nullptr || Callback.Function.val_ == nullptr) {
    OC_LOG("panorama", "subscribe needs a function");
    return std::nullopt;
  }
  const std::int32_t Id =
      engine()->RegisterJSGenericCallback(Menu, Callback.Function);
  Subscribers.push_back(Id);
  return Id;
}

void unsubscribe(std::int32_t Id) {
  const auto Found = std::ranges::find(Subscribers, Id);
  if (Found == Subscribers.end()) {
    return;
  }
  Subscribers.erase(Found);
  engine()->UnregisterJSGenericCallback(Id);
}

JsMethod<&subscribe> Subscribe{"events", "subscribe", "callback"};
JsMethod<&unsubscribe> Unsubscribe{"events", "unsubscribe", "id"};

void invokeSubscribers(std::string_view Feature, std::string_view Event) {
  if (Subscribers.empty()) {
    return;
  }
  CUIEngine *const Engine = engine();
  v8::Isolate *const Isolate = Engine->GetIsolate();
  const std::vector<std::int32_t> Targets = Subscribers;
  v8::Isolate::Enter(Isolate);
  {
    const v8::HandleScope Scope(Isolate);
    std::array<v8::Local<v8::Value>, NotifyArguments> Arguments{
        v8::String::fromUtf8(Isolate, Feature),
        v8::String::fromUtf8(Isolate, Event),
    };
    for (const std::int32_t Id : Targets) {
      if (!Engine->InvokeJSGenericCallback(Id, NotifyArguments,
                                           Arguments.data(), nullptr)) {
        OC_LOG("panorama", "subscriber {} not invoked", Id);
      }
    }
  }
  v8::Isolate::Exit(Isolate);
}

bool registerFunction(v8::FunctionCallback Callback) {
  CUIEngine *const Engine = engine();
  const v8::Global<v8::Context> *const Main = Engine->GetMainContext();
  if (Main->val_ == nullptr) {
    return false;
  }
  v8::Isolate *const Isolate = Engine->GetIsolate();
  v8::Isolate::Enter(Isolate);
  {
    const v8::HandleScope Scope(Isolate);
    const v8::Local<v8::Context> Context =
        v8::HandleScope::local(Isolate, *Main);
    v8::Context::Enter(Context.val_);
    v8::Local<v8::FunctionTemplate> Template;
    v8::FunctionTemplate::New(
        &Template, Isolate, Callback, v8::Local<v8::Value>(),
        v8::Local<v8::Signature>(), std::int32_t{0},
        v8::ConstructorBehavior::kAllow, v8::SideEffectType::kHasSideEffect,
        static_cast<const v8::CFunction *>(nullptr), std::uint16_t{0},
        std::uint16_t{0}, std::uint16_t{0});
    Engine->RegisterJSGlobalFunction(FunctionName, &Template, true);
    v8::Context::Exit(Context.val_);
  }
  v8::Isolate::Exit(Isolate);
  return true;
}

bool addStyleFile(CUIPanel *Root, std::string_view Css) {
  CUIEngine *const Engine = engine();
  if (!Engine->GetLayoutManager()->addFileOverride(
          Engine->makePathSymbol(StylePath), Css)) {
    OC_LOG("panorama", "{} is already in memory", StylePath);
    return false;
  }
  const std::array Paths{StyleUrl};
  return Root->AddStyleFiles(Paths.data(),
                             static_cast<std::int32_t>(Paths.size()), false);
}

} // namespace

bool startPanorama(std::string_view Css, const char *Script) {
  CUIPanel *const Menu = mainMenu();
  if (Menu == nullptr || !registerFunction(&dispatchJs)) {
    OC_LOG("panorama", "no main menu panel or context");
    return false;
  }
  Started = true;
  ++Starts;
  run(Menu, "openchanger/api.js", apiScript().c_str());
  run(Menu, "openchanger/bundle.js", Script);
  CUIPanel *const Root = Menu->FindChildTraverse(rootId().c_str());
  if (Root == nullptr || !addStyleFile(Root, Css)) {
    OC_LOG("panorama", "our style file is not attached");
    return false;
  }
  OC_LOG("panorama", "{} registered, {} methods", FunctionName,
         jsMethods().size());
  return true;
}

void stopPanorama() {
  if (!Started) {
    return;
  }
  Started = false;
  invokeSubscribers("ui", "unload");
  CUIEngine *const Engine = engine();
  for (const std::int32_t Id : Subscribers) {
    Engine->UnregisterJSGenericCallback(Id);
  }
  OC_LOG("panorama", "{} subscribers unregistered", Subscribers.size());
  Subscribers.clear();
  CUIPanel *const Menu = mainMenu();
  if (Menu != nullptr) {
    if (CUIPanel *const Root = Menu->FindChildTraverse(rootId().c_str())) {
      Root->ClearStyleFiles();
    }
    run(Menu, "openchanger/stop.js", StopScript);
  }
  Engine->PurgeUnusedFiles();
  Engine->GetLayoutManager()->DiscardEdits();
  if (!registerFunction(nullptr)) {
    OC_LOG("panorama", "{} is not replaced: no main context", FunctionName);
    return;
  }
  OC_LOG("panorama", "{} replaced with an inert function", FunctionName);
}

void notify(std::string_view Feature, std::string_view Event) {
  struct Notification {
    std::string_view Feature;
    std::string_view Event;
  } Note{.Feature = Feature, .Event = Event};
  const auto Send = [](void *Context) {
    const auto *const Sent = static_cast<const Notification *>(Context);
    invokeSubscribers(Sent->Feature, Sent->Event);
  };
  if (!SDL::RunOnMainThread(Send, &Note, true)) {
    OC_LOG("panorama", "{}.{} not sent", Feature, Event);
  }
}

} // namespace oc

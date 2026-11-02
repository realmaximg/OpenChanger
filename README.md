# OpenChanger

A Windows x64 DLL for Counter-Strike 2 that adds client-side features to the game. The first feature is a cosmetics
changer: skins, knives, gloves, agents, music kits, medals, stickers, patches and keychains, picked in an OpenChanger
tab of the game's own main menu. Everything happens on your own client. Settings are saved to
`%APPDATA%\openchanger\changer.json`.

## License

MIT, see `LICENSE`. The libraries in `thirdparty/` keep their own license files in their folders; the frontend's
third-party code is listed in `thirdparty/NOTICES.md`.

## Requirements

* Windows x64.
* LLVM 19+ in `C:\Program Files\LLVM` (`winget install --id LLVM.LLVM -e`). `tools\build.cmd` puts its `bin` first
  in `PATH`; clang-cl and lld-link build everything.
* Visual Studio Build Tools 2022 with the C++ x64 tools: only for the Windows SDK, the STL and the CRT (the MSVC
  compiler is not used). `tools\build.cmd` finds them through `vswhere`.
* CMake 3.25+ and Ninja.
* Node.js 24+ with npm, for the GUI.

## Build

```
tools\build.cmd dev        # build\dev\openchanger.dll: with the dev log
tools\build.cmd release    # build\release\openchanger.dll: no exports, the minified GUI embedded
tools\build.cmd lint       # clang-format and clang-tidy over src\, 0 findings expected
```

The first build of each preset also builds protobuf 3.21.8 into `%LOCALAPPDATA%\openchanger\`, which takes a few
minutes. Every dev build writes `src/ui/panorama/generated/api.d.ts`: the TypeScript types of the C++ methods the GUI
calls, printed by the dev-only program `oc_api_types` from the same objects as the DLL. `tools\build.cmd release`
works on a fresh clone: it builds that program with the dev preset first, then the minified GUI (`npm ci` on first
use, then `npm run build:release`), then the DLL with the GUI embedded.

The GUI is a TypeScript project in `src/ui/panorama`. The dev DLL reads it from `src/ui/panorama/dist/` when it
starts, so build the dev DLL once, then the GUI:

```
cd src\ui\panorama
npm ci
npm run build              # or: npm run watch
npm run check              # api.d.ts present, tsc and Biome
```

A changed C++ method changes `api.d.ts`, and `npm run check` fails until the GUI follows it.

## Debugging

The dev build writes `%APPDATA%\openchanger\dev\log.txt`, one line per event, `| <part> | <text>`:

* At start, every link (see «Game access» below) is listed as `Class::Name = 0x...` or `missing Class::Name`. Links
  are resolved kind by kind: exports, interfaces, patterns, game systems, schema fields, finders. When a kind has a
  missing link, the later kinds are not tried, the log says `N links missing, not started`, and the DLL does nothing.
  After a game update this is where a pattern that no longer matches shows up.
* A feature that cannot start says why (`changer did not start`, with the reason above it), and nothing is hooked.
* Errors in the GUI's code go to the same log as `| js |` lines with their stack. `| js | styles parsed to the end`,
  written when the tab is first shown, means the whole `bundle.css` was accepted by Panorama.

## Code map

```
src/
  core/      no game: log, settings, Published<T>, Feature, text
  game/      our access to the game: modules, patterns, links, hooks, slot finders; the services for features: the
             decode handler, game events, the JS bridge (js) and the Panorama tab (panorama)
  sdk/       game classes, one header per class, grouped by module (client/, engine2/, panorama/, v8/, ...)
  features/  one folder per feature: changer/
  ui/        ui.cpp (the GUI's files, start, stop) and each feature's JS methods: changer/methods.cpp;
             ui/panorama/ is the frontend project
  app/       DllMain, start and unload, the feature list; dev.cpp: the dev log at start and the dev exports
proto/       the game's protobuf messages and ours (openchanger.proto), compiled at build time
cmake/       compiler flags, the protobuf build
thirdparty/  glaze, libhat, safetyhook with Zydis, protobuf
tools/       build.cmd
```

Dependencies go down only: `app` -> `ui` -> `features` -> `game`, `sdk` -> `core`. A feature never includes another
feature, `ui` or `app`. `sdk` uses only the lower part of `game` (module, pattern, link, hook, find).

Where to start reading:

* `src/app/dllmain.cpp` `start()`: links, features, hooks, GUI, in this order; `unhook()` and `stop()` undo them.
* `src/game/link.cpp` `resolveLinks()` and `src/sdk/macros.h`: how a game value becomes a link.
* `src/features/changer/Changer.cpp`: the feature's start, stop and API.
* `src/features/changer/skins/decoded.cpp`: what the match shows (my controller, pawn and weapons).
* `src/features/changer/inventory/socache.cpp`: what the menu shows (our objects in the game's inventory cache).
* `src/ui/changer/methods.cpp`: what the GUI may call.
* `src/ui/panorama/src/main.tsx`: the frontend's entry.

The changer has three folders, one per layer: `src/features/changer/` (the feature), `src/ui/changer/` (its JS
methods) and `src/ui/panorama/src/features/changer/` (its page). Inside the feature:

```
Changer.h/.cpp        start, stop, level end, saving, the API
loadout/              ItemRecord, Loadout, our item ids, quality; validate (pure)
catalog/              catalog types and lookups (pure); build.cpp reads the game's item schema
inventory/            objects (pure: records -> CSOEconItem / CSOEconEquipSlot, where an item is equipped);
                      socache.cpp (the sync on the main thread) and its four hooks
skins/decoded.cpp     the end of each entity's decode: my controller, pawn, weapons
inspect/              the knife inspect hook
voice/                voices (pure: the translation table), the sound hook, the HE line on grenade_thrown
stattrak/             player_death (StatTrak of my weapon, and our knife's icon in the kill feed), round_mvp
```

A file name says what is inside: a hook is named after the game function it hooks (`SOCreated.cpp`), a game-event
handler after the event (`player_death.cpp`), a class after the class (`DecodedHandler.h`, `C_BaseEntity.h`,
`Changer.h`), anything else after our concept in lowercase (`socache.cpp`, `objects.cpp`). Pure files (no game, no
`windows.h`) go into the feature's static library `oc_<feature>`; everything that includes `game` or `sdk` or
registers itself (links, hooks, handlers, JS methods) goes into the object library `oc_objects`, because a static
library would drop an object nobody references, and its registration with it.

## How it works

One approach is used everywhere:

* Everything taken from the game is a link: a static object declared next to the game class it belongs to (a
  pattern, an export, an interface, a game system, a schema field, a finder). All links are resolved at start; if any
  is missing, the log lists it and the DLL stays inactive. There is no partial operation.
* Hooks are static objects next to their detours, one file per game function, installed after the features start.
  Detours run in the game's own threads; state they share across threads is published through `Published<T>`.
* Start, unload and any work that needs the game's main thread go through SDL's `SDL_RunOnMainThread`. There are no
  threads of our own, no timers and no per-frame polling.
* Values are put in at the end of an entity's decode, or through the game's own inventory cache, so the game renders
  them with its own code.
* The GUI calls C++ through one V8 function; C++ tells the GUI about a change with `oc::notify(feature, event)`.

## Conventions

* C++23 with clang-cl, LLVM style (`.clang-format`, `.clang-tidy`; `tools\build.cmd lint` must report 0 findings).
  Types, variables and members are `UpperCamelCase`, functions `lowerCamelCase`. `src/sdk` keeps the game's names
  (`C_BaseEntity`, `m_iTeamNum`).
* No comments, no exceptions, no RTTI. A function that can fail returns `std::expected<T, std::string>`, or `bool` /
  a pointer when there is nothing to say. An empty game object (no own player in the menu) is normal: check and skip.
* Game knowledge (layouts, naming rules, how to find and read the game's data) lives in `src/sdk`; a feature holds
  only its own decisions. No SEH and no guarded reads: memory is read only along chains the game gives.
* Development code (the log, the dev exports, `OpenChangerAPI.dev`) exists only under `OPENCHANGER_DEV`.
* Frontend: Solid components, motion only in CSS on the tokens of `design/tokens.ts`, every text in `i18n/`.

## Recipes

### A game field or function

Add it to the header of its game class in `src/sdk/<module>/`, with a macro from `src/sdk/macros.h`:

| What | Macro |
|---|---|
| a field the schema system knows | `SCHEMA_FIELD(Type, m_name)` |
| a field outside the schema | `PATTERN_FIELD(Module, Type, Name, PtName)` |
| a bare value of a pattern (a record size) | `SDK_PATTERN_LINK(Module, Name, PtName)`, read as `NameLink.value()` |
| a virtual function with a call site | `PATTERN_VIRTUAL(Module, Signature, Name, PtName)` |
| a non-virtual function | `PATTERN_FUNCTION(Module, Signature, Name, PtName)` |
| a global | `PATTERN_GLOBAL(Module, Type, Name, PtName)` |
| an engine interface / a game system / an export | `SDK_INTERFACE`, `SDK_GAME_SYSTEM`, `SDK_EXPORT` (give `static get()`) |
| an exported function | `SDK_EXPORT_FUNCTION(Module, Signature, Name, "decorated name")` |
| a value no single pattern gives | `FIELD_FIND`, `VIRTUAL_FIND`, `FUNCTION_FIND`, `GLOBAL_FIND` with a finder |
| a virtual function that cannot be found by what it does | `VIRTUAL_INDEX(Signature, Name, slot)` (last resort) |
| a layout fixed by the ABI or a library | members, `PAD(n)`, `static_assert(offsetof(...))` |

`FUNCTION_FIND`, `GLOBAL_FIND` and `VIRTUAL_INDEX` have no user yet; they are kept so that every kind of value has
its macro. A finder is a capture-less function returning an optional value; the shared helpers are in `game/find.h`
(`vtableOf`, `findSlot`, `findSlotMatching`: the one slot whose function matches a pattern, `findSlotNamed`: the slot
whose pattern points at a name string) and `game/pattern.h`.

A pattern is a `#define Pt<Name>` at the top of the class's header (`Pt<Class>_<Name>` for a second class in the same
header). `?` is any byte, `*` marks the value bytes (one run of 1 or 4). It must match the module's `.text` exactly
once. For `PATTERN_FUNCTION` and `PATTERN_GLOBAL`, four `*` are a rel32 and the value is its target, e.g.
`E8 * * * *` at a unique call site; without `*` the value is the match address.

Non-virtual game functions are static and take the object first:
`CNetworkGameClient::ForceFullUpdate(Client, "reason")`. Virtual ones are members: `engine()->RunScript(...)`. A
member wrapper exists only when it adds a game rule (`CGCClientSharedObjectCache::put`, `IGameEvent::isLocalPlayer`).

### A hook

One file per game function, named after it, in the folder of the part that needs it:

```cpp
namespace oc::changer {

namespace {

void detour(CPlayerInventory *Inventory, const GCSDK::SOID_t *Owner,
            GCSDK::CSharedObject *Object, GCSDK::ESOCacheEvent Event);

InlineHook<detour> Hook{CPlayerInventory::SOCreatedLink};

void detour(CPlayerInventory *Inventory, const GCSDK::SOID_t *Owner,
            GCSDK::CSharedObject *Object, GCSDK::ESOCacheEvent Event) {
  Hook.original(Inventory, Owner, Object, Event);
  realSlotChanged(*Owner, *Object, Event);
}

} // namespace

} // namespace oc::changer
```

The hook lives in its layer's namespace (`oc` in `game` and `app`, `oc::<feature>` in a feature) inside an anonymous
namespace. `InlineHook` takes a function link, `VirtualHook<detour> Hook{Class::get, Class::MethodLink}` a vtable
slot. The original's signature comes from the detour; `app` installs and removes all hooks at once. A detour that
runs while the unload waits for it must not call `oc::notify` (it waits for the main thread).

### A game event or the end of a decode

```cpp
GameEventHandler Handler{"round_mvp", onRoundMvp};   // game/GameEventHandler.h, void(IGameEvent &)
DecodedHandler Handler{onDecoded};                    // game/DecodedHandler.h, after each entity's decode
```

Both are static objects in the file that handles them, compiled into `oc_objects`.

### A JS method

```cpp
std::string equip(ItemRecord Record) { ... }

JsMethod<&equip> Equip{"changer", "equip", "record"};   // game/js.h
```

JS calls `OpenChangerAPI.changer.equip(record)`; one name per parameter, checked at compile time. Arguments and
results are converted by type: `bool`, numbers, `std::string`, `std::vector`, `std::optional` (`undefined`), an enum
with a `JsEnum<E>` name list, an aggregate struct (an object by member names). The next dev build prints the method
into `api.d.ts`. A JS type is the feature's own type; a struct in `ui/` exists only when it reshapes the feature's
data for the GUI (`ui::Item`, `ui::LoadoutItem`). After a change the GUI must see, the method calls
`notify("changer", "loadoutChanged")`, and the frontend listens with `on('changer.loadoutChanged', ...)`.

### A frontend page and its state

* A feature's page is one call in `src/ui/panorama/src/main.tsx`: `addPage({id, title, icon, Page})`.
* C++ is called only from the feature's modules (`api.ts` and the module that owns the data), never from a screen.
  C++ data lives in `store.ts` (loadout, favorites), `catalog.ts` and `kits.ts` (cached lists); screen state lives
  in the module of its part (`browser.ts`, `editor.ts`, `kits.ts`, `scene.ts`, `layout.ts`, `toast.ts`).
* `api.ts`, `store.ts` and `editor.ts` read C++ when they are imported, so they can be imported only after the bridge
  has created `OpenChangerAPI`, which it does before it runs the bundle.

### A feature

1. `src/features/<name>/` with `<Name>.h/.cpp`: `class <Name> final : public oc::Feature` with `static get()`; its
   public methods are its API. Its parts go into subfolders, its settings through `loadSettings<T>(*this)` /
   `saveSettings(*this, Value)` (`%APPDATA%\openchanger\<name()>.json`).
2. `src/features/<name>/CMakeLists.txt`: the pure files into `oc_<name>`, the game files into `oc_objects`; one
   `add_subdirectory(src/features/<name>)` line in the root `CMakeLists.txt`.
3. One line in `src/app/features.cpp`.
4. Its JS methods in `src/ui/<name>/methods.cpp`, added to the `oc_objects` list in the root `CMakeLists.txt`.
5. Its page in `src/ui/panorama/src/features/<name>/` and one `addPage(...)` line in `src/ui/panorama/src/main.tsx`.

#include "game/hook.h"

#include <algorithm>
#include <atomic>
#include <ranges>
#include <vector>

namespace oc {

namespace {

std::atomic<long> ActiveCalls{0};
std::atomic<bool> Draining{false};

std::vector<Hook *> &registry() {
  static std::vector<Hook *> Hooks;
  return Hooks;
}

} // namespace

Hook::CallGuard::CallGuard() noexcept { ActiveCalls.fetch_add(1); }

Hook::CallGuard::~CallGuard() {
  if (ActiveCalls.fetch_sub(1) == 1 && Draining.load()) {
    ActiveCalls.notify_all();
  }
}

Hook::Hook() noexcept { registry().push_back(this); }

bool installHooks() {
  return std::ranges::all_of(registry(),
                             [](Hook *Entry) { return Entry->install(); });
}

void removeHooks() {
  for (Hook *const Entry : std::views::reverse(registry())) {
    Entry->remove();
  }
  Draining.store(true);
  for (long Active = ActiveCalls.load(); Active != 0;
       Active = ActiveCalls.load()) {
    ActiveCalls.wait(Active);
  }
}

} // namespace oc

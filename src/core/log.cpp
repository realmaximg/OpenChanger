#include "core/log.h"

#ifdef OPENCHANGER_DEV

#include "core/settings.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <mutex>
#include <ostream>
#include <string>
#include <system_error>

namespace oc {

namespace {

std::ofstream openLog() {
  const auto Directory = userDirectory() / L"dev";
  std::error_code Error;
  std::filesystem::create_directories(Directory, Error);
  return std::ofstream(Directory / L"log.txt", std::ios::app);
}

} // namespace

void logLine(const char *Part, const std::string &Text) {
  static std::mutex Mutex;
  static std::ofstream File = openLog();
  const auto Now = std::chrono::system_clock::now();
  const std::time_t Seconds = std::chrono::system_clock::to_time_t(Now);
  std::tm Local{};
  localtime_s(&Local, &Seconds);
  const auto Milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          Now.time_since_epoch())
          .count() %
      1000;
  const std::scoped_lock Lock(Mutex);
  File << std::format("{:02}:{:02}:{:02}.{:03} | {} | {}\n", Local.tm_hour,
                      Local.tm_min, Local.tm_sec, Milliseconds, Part, Text)
       << std::flush;
}

} // namespace oc

#endif

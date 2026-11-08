#include "pos/infrastructure/system_clock.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace pos {

std::string SystemClock::today() const {
  const std::time_t now = std::time(nullptr);
  std::tm local{};
#ifdef _WIN32
  localtime_s(&local, &now);
#else
  localtime_r(&now, &local);
#endif
  std::ostringstream out;
  out << std::put_time(&local, "%Y-%m-%d");
  return out.str();
}

}  // namespace pos

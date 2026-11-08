#pragma once

#include "pos/application/ports.hpp"

namespace pos {

class SystemClock : public Clock {
 public:
  std::string today() const override;
};

}  // namespace pos

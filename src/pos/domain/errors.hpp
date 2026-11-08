#pragma once
/// Доменные ошибки: нарушение инвариантов модели.

#include <stdexcept>
#include <string>

namespace pos {

class DomainError : public std::runtime_error {
 public:
  explicit DomainError(const std::string& message) : std::runtime_error(message) {}
};

}  // namespace pos

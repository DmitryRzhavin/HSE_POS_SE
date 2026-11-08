#include "pos/domain/text.hpp"

#include <cctype>

#include "pos/domain/errors.hpp"

namespace pos {

std::string normalize_text(const std::string& value, const std::string& field_name) {
  std::size_t begin = 0;
  std::size_t end = value.size();
  while (begin < end && std::isspace(static_cast<unsigned char>(value[begin]))) {
    ++begin;
  }
  while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
    --end;
  }
  if (begin == end) {
    throw DomainError(field_name + " не может быть пустым");
  }
  return value.substr(begin, end - begin);
}

}  // namespace pos

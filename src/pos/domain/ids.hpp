#pragma once
/// Идентификаторы студента и критерия (Value Object'ы).

#include <string>

namespace pos {

class StudentId {
 public:
  explicit StudentId(const std::string& value);

  const std::string& value() const { return value_; }
  bool operator==(const StudentId& other) const { return value_ == other.value_; }
  bool operator<(const StudentId& other) const { return value_ < other.value_; }

 private:
  std::string value_;
};

class CriterionId {
 public:
  explicit CriterionId(const std::string& value);

  const std::string& value() const { return value_; }
  bool operator==(const CriterionId& other) const { return value_ == other.value_; }
  bool operator<(const CriterionId& other) const { return value_ < other.value_; }

 private:
  std::string value_;
};

}  // namespace pos

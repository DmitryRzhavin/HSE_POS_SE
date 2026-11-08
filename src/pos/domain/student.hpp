#pragma once
/// Студент — Entity: идентичность определяется StudentId.

#include <string>

#include "pos/domain/ids.hpp"

namespace pos {

class Student {
 public:
  Student(const StudentId& id, const std::string& full_name);

  const StudentId& id() const { return id_; }
  const std::string& full_name() const { return full_name_; }

 private:
  StudentId id_;
  std::string full_name_;
};

}  // namespace pos

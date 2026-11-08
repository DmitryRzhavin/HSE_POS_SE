#pragma once
/// Сценарии со студентами.

#include <memory>
#include <string>
#include <vector>

#include "pos/application/ports.hpp"

namespace pos {

class RosterService {
 public:
  explicit RosterService(std::shared_ptr<StudentRepository> students);

  Student register_student(const std::string& id, const std::string& full_name);
  std::vector<Student> list_students() const;

 private:
  std::shared_ptr<StudentRepository> students_;
};

}  // namespace pos

#include "pos/infrastructure/in_memory_repositories.hpp"

namespace pos {

void InMemoryStudentRepository::add(const Student& student) { students_.push_back(student); }

std::optional<Student> InMemoryStudentRepository::find(const StudentId& id) const {
  for (const auto& student : students_) {
    if (student.id() == id) {
      return student;
    }
  }
  return std::nullopt;
}

std::vector<Student> InMemoryStudentRepository::all() const { return students_; }

}  // namespace pos

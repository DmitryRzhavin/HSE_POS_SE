#include "pos/application/roster_service.hpp"

#include "pos/domain/errors.hpp"

namespace pos {

RosterService::RosterService(std::shared_ptr<StudentRepository> students)
    : students_(std::move(students)) {}

Student RosterService::register_student(const std::string& id, const std::string& full_name) {
  Student student(StudentId(id), full_name);
  if (students_->find(student.id())) {
    throw DomainError("Студент '" + student.id().value() + "' уже зарегистрирован");
  }
  students_->add(student);
  return student;
}

std::vector<Student> RosterService::list_students() const { return students_->all(); }

}  // namespace pos

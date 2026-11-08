#include "pos/domain/student.hpp"

#include "pos/domain/text.hpp"

namespace pos {

Student::Student(const StudentId& id, const std::string& full_name)
    : id_(id), full_name_(normalize_text(full_name, "Имя студента")) {}

}  // namespace pos

#pragma once
/// Адаптеры хранения: данные живут в памяти до завершения процесса.

#include <map>

#include "pos/application/ports.hpp"

namespace pos {

class InMemoryStudentRepository : public StudentRepository {
 public:
  void add(const Student& student) override;
  std::optional<Student> find(const StudentId& id) const override;
  std::vector<Student> all() const override;

 private:
  std::vector<Student> students_;
};

class InMemoryRubricRepository : public RubricRepository {
 public:
  Rubric load() const override { return rubric_; }
  void save(const Rubric& rubric) override { rubric_ = rubric; }

 private:
  Rubric rubric_;
};

class InMemoryGradebookRepository : public GradebookRepository {
 public:
  Gradebook load() const override { return gradebook_; }
  void save(const Gradebook& gradebook) override { gradebook_ = gradebook; }

 private:
  Gradebook gradebook_;
};

}  // namespace pos

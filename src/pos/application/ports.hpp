#pragma once
/// Порты: абстракции, которые application-слой требует от внешнего мира.
/// Реализации (адаптеры) живут в infrastructure/ и зависят от этих интерфейсов.

#include <optional>
#include <string>
#include <vector>

#include "pos/domain/gradebook.hpp"
#include "pos/domain/rubric.hpp"
#include "pos/domain/student.hpp"

namespace pos {

class StudentRepository {
 public:
  virtual ~StudentRepository() = default;
  virtual void add(const Student& student) = 0;
  virtual std::optional<Student> find(const StudentId& id) const = 0;
  virtual std::vector<Student> all() const = 0;
};

class RubricRepository {
 public:
  virtual ~RubricRepository() = default;
  virtual Rubric load() const = 0;
  virtual void save(const Rubric& rubric) = 0;
};

class GradebookRepository {
 public:
  virtual ~GradebookRepository() = default;
  virtual Gradebook load() const = 0;
  virtual void save(const Gradebook& gradebook) = 0;
};

class Clock {
 public:
  virtual ~Clock() = default;
  /// Текущая дата в формате ГГГГ-ММ-ДД.
  virtual std::string today() const = 0;
};

/// Строка выгрузки итогов — DTO для внешних форматов.
struct GradeReportRow {
  std::string student_id;
  std::string full_name;
  double percent;
  double ten_point;
  std::size_t evaluated_count;
  std::size_t criteria_count;
};

class GradeReportWriter {
 public:
  virtual ~GradeReportWriter() = default;
  virtual void write(const std::string& destination, const std::vector<GradeReportRow>& rows) = 0;
};

}  // namespace pos

#pragma once
/// Сценарии выставления, исправления и отмены оценок.

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "pos/application/ports.hpp"

namespace pos {

struct AssignmentResult {
  bool replaced;  // true — исправлена ранее выставленная оценка
};

class GradingService {
 public:
  GradingService(std::shared_ptr<StudentRepository> students,
                 std::shared_ptr<RubricRepository> rubric,
                 std::shared_ptr<GradebookRepository> gradebook, std::shared_ptr<Clock> clock);

  /// Выставляет оценку; повторный вызов для той же пары исправляет её.
  AssignmentResult assign_score(const std::string& student_id, const std::string& criterion_id,
                                double value, const std::string& comment = "");
  /// Возвращает false, если отменять было нечего.
  bool cancel_score(const std::string& student_id, const std::string& criterion_id);
  std::vector<Evaluation> list_evaluations(const std::optional<std::string>& student_id) const;

 private:
  std::shared_ptr<StudentRepository> students_;
  std::shared_ptr<RubricRepository> rubric_;
  std::shared_ptr<GradebookRepository> gradebook_;
  std::shared_ptr<Clock> clock_;
};

}  // namespace pos

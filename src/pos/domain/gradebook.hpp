#pragma once
/// Ведомость — корень агрегата: все оценки курса.
/// Инвариант: не больше одной оценки на пару (студент, критерий).

#include <map>
#include <utility>
#include <vector>

#include "pos/domain/evaluation.hpp"

namespace pos {

class Gradebook {
 public:
  /// Возвращает true, если существующая оценка была заменена (исправление).
  bool put(const Evaluation& evaluation);
  /// Возвращает true, если оценка существовала и была отменена.
  bool cancel(const StudentId& student_id, const CriterionId& criterion_id);

  const Evaluation* find(const StudentId& student_id, const CriterionId& criterion_id) const;
  std::vector<Evaluation> for_student(const StudentId& student_id) const;
  std::vector<Evaluation> all() const;

 private:
  using Key = std::pair<StudentId, CriterionId>;
  std::map<Key, Evaluation> evaluations_;
};

}  // namespace pos

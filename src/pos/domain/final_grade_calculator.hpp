#pragma once
/// Domain Service: расчёт итога не принадлежит ни студенту, ни оценке, ни рубрикатору.

#include <memory>

#include "pos/domain/final_grade.hpp"
#include "pos/domain/gradebook.hpp"
#include "pos/domain/grading_policy.hpp"

namespace pos {

class FinalGradeCalculator {
 public:
  explicit FinalGradeCalculator(std::shared_ptr<const GradingPolicy> policy);

  FinalGrade calculate(const Rubric& rubric, const Gradebook& gradebook,
                       const StudentId& student_id) const;
  const GradingPolicy& policy() const { return *policy_; }

 private:
  std::shared_ptr<const GradingPolicy> policy_;
};

}  // namespace pos

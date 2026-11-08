#include "pos/domain/final_grade_calculator.hpp"

#include "pos/domain/errors.hpp"

namespace pos {

FinalGradeCalculator::FinalGradeCalculator(std::shared_ptr<const GradingPolicy> policy)
    : policy_(std::move(policy)) {
  if (!policy_) {
    throw DomainError("Для расчёта итога нужна политика");
  }
}

FinalGrade FinalGradeCalculator::calculate(const Rubric& rubric, const Gradebook& gradebook,
                                           const StudentId& student_id) const {
  const std::vector<Evaluation> evaluations = gradebook.for_student(student_id);
  return FinalGrade{student_id, policy_->percent(rubric, evaluations), evaluations.size(),
                    rubric.criteria().size()};
}

}  // namespace pos

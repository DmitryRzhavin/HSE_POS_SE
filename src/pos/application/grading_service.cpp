#include "pos/application/grading_service.hpp"

#include "pos/domain/errors.hpp"

namespace pos {

GradingService::GradingService(std::shared_ptr<StudentRepository> students,
                               std::shared_ptr<RubricRepository> rubric,
                               std::shared_ptr<GradebookRepository> gradebook,
                               std::shared_ptr<Clock> clock)
    : students_(std::move(students)), rubric_(std::move(rubric)),
      gradebook_(std::move(gradebook)), clock_(std::move(clock)) {}

AssignmentResult GradingService::assign_score(const std::string& student_id,
                                              const std::string& criterion_id, double value,
                                              const std::string& comment) {
  const StudentId student(student_id);
  if (!students_->find(student)) {
    throw DomainError("Студента '" + student.value() + "' нет в списке");
  }
  const Evaluation evaluation = EvaluationFactory::create(
      rubric_->load(), student, CriterionId(criterion_id), value, comment, clock_->today());
  Gradebook gradebook = gradebook_->load();
  const bool replaced = gradebook.put(evaluation);
  gradebook_->save(gradebook);
  return AssignmentResult{replaced};
}

bool GradingService::cancel_score(const std::string& student_id, const std::string& criterion_id) {
  Gradebook gradebook = gradebook_->load();
  const bool cancelled = gradebook.cancel(StudentId(student_id), CriterionId(criterion_id));
  if (cancelled) {
    gradebook_->save(gradebook);
  }
  return cancelled;
}

std::vector<Evaluation> GradingService::list_evaluations(
    const std::optional<std::string>& student_id) const {
  const Gradebook gradebook = gradebook_->load();
  return student_id ? gradebook.for_student(StudentId(*student_id)) : gradebook.all();
}

}  // namespace pos

#include "pos/domain/evaluation.hpp"

#include "pos/domain/text.hpp"

namespace pos {

Evaluation EvaluationFactory::create(const Rubric& rubric, const StudentId& student_id,
                                     const CriterionId& criterion_id, double value,
                                     const std::string& comment, const std::string& date) {
  const Criterion& criterion = rubric.get(criterion_id);
  return Evaluation(student_id, criterion_id, Score::of(value, criterion), comment,
                    normalize_text(date, "Дата оценки"));
}

}  // namespace pos

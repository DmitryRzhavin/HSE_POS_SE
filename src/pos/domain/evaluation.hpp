#pragma once
/// Оценка — Entity: «студенту по критерию поставили балл».
/// Идентичность — пара (студент, критерий). Создаётся только через EvaluationFactory.

#include <string>

#include "pos/domain/ids.hpp"
#include "pos/domain/rubric.hpp"
#include "pos/domain/score.hpp"

namespace pos {

class Evaluation {
 public:
  const StudentId& student_id() const { return student_id_; }
  const CriterionId& criterion_id() const { return criterion_id_; }
  const Score& score() const { return score_; }
  const std::string& comment() const { return comment_; }
  const std::string& date() const { return date_; }

 private:
  friend class EvaluationFactory;

  Evaluation(const StudentId& student_id, const CriterionId& criterion_id, const Score& score,
             const std::string& comment, const std::string& date)
      : student_id_(student_id), criterion_id_(criterion_id), score_(score), comment_(comment),
        date_(date) {}

  StudentId student_id_;
  CriterionId criterion_id_;
  Score score_;
  std::string comment_;
  std::string date_;
};

/// Фабрика (Factory): единственная точка создания оценки с проверкой инвариантов
/// «критерий есть в рубрикаторе» и «балл в пределах критерия».
class EvaluationFactory {
 public:
  static Evaluation create(const Rubric& rubric, const StudentId& student_id,
                           const CriterionId& criterion_id, double value,
                           const std::string& comment, const std::string& date);
};

}  // namespace pos

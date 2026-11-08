#include "pos/domain/gradebook.hpp"

namespace pos {

bool Gradebook::put(const Evaluation& evaluation) {
  Key key{evaluation.student_id(), evaluation.criterion_id()};
  auto it = evaluations_.find(key);
  if (it != evaluations_.end()) {
    it->second = evaluation;
    return true;
  }
  evaluations_.emplace(key, evaluation);
  return false;
}

bool Gradebook::cancel(const StudentId& student_id, const CriterionId& criterion_id) {
  return evaluations_.erase(Key{student_id, criterion_id}) > 0;
}

const Evaluation* Gradebook::find(const StudentId& student_id,
                                  const CriterionId& criterion_id) const {
  auto it = evaluations_.find(Key{student_id, criterion_id});
  return it == evaluations_.end() ? nullptr : &it->second;
}

std::vector<Evaluation> Gradebook::for_student(const StudentId& student_id) const {
  std::vector<Evaluation> result;
  for (const auto& entry : evaluations_) {
    if (entry.first.first == student_id) {
      result.push_back(entry.second);
    }
  }
  return result;
}

std::vector<Evaluation> Gradebook::all() const {
  std::vector<Evaluation> result;
  for (const auto& entry : evaluations_) {
    result.push_back(entry.second);
  }
  return result;
}

}  // namespace pos

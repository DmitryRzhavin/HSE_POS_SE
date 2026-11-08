#include "pos/application/rubric_service.hpp"

namespace pos {

RubricService::RubricService(std::shared_ptr<RubricRepository> rubric)
    : rubric_(std::move(rubric)) {}

Criterion RubricService::define_criterion(const std::string& id, const std::string& title,
                                          double max_score, double weight) {
  Criterion criterion(CriterionId(id), title, max_score, weight);
  Rubric rubric = rubric_->load();
  rubric.add_criterion(criterion);
  rubric_->save(rubric);
  return criterion;
}

std::vector<Criterion> RubricService::list_criteria() const {
  return rubric_->load().criteria();
}

}  // namespace pos

#include "pos/domain/rubric.hpp"

#include "pos/domain/errors.hpp"

namespace pos {

void Rubric::add_criterion(const Criterion& criterion) {
  if (find(criterion.id()) != nullptr) {
    throw DomainError("Критерий '" + criterion.id().value() + "' уже есть в рубрикаторе");
  }
  criteria_.push_back(criterion);
}

const Criterion* Rubric::find(const CriterionId& id) const {
  for (const auto& criterion : criteria_) {
    if (criterion.id() == id) {
      return &criterion;
    }
  }
  return nullptr;
}

const Criterion& Rubric::get(const CriterionId& id) const {
  const Criterion* criterion = find(id);
  if (criterion == nullptr) {
    throw DomainError("Критерия '" + id.value() + "' нет в рубрикаторе");
  }
  return *criterion;
}

double Rubric::total_weight() const {
  double total = 0;
  for (const auto& criterion : criteria_) {
    total += criterion.weight();
  }
  return total;
}

}  // namespace pos

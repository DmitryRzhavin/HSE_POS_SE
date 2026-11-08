#include "pos/domain/criterion.hpp"

#include <cmath>

#include "pos/domain/errors.hpp"
#include "pos/domain/text.hpp"

namespace pos {

Criterion::Criterion(const CriterionId& id, const std::string& title, double max_score,
                     double weight)
    : id_(id), title_(normalize_text(title, "Название критерия")), max_score_(max_score),
      weight_(weight) {
  if (!std::isfinite(max_score) || max_score <= 0) {
    throw DomainError("Максимальный балл критерия должен быть положительным");
  }
  if (!std::isfinite(weight) || weight <= 0) {
    throw DomainError("Вес критерия должен быть положительным");
  }
}

bool Criterion::accepts(double score) const {
  return std::isfinite(score) && score >= 0 && score <= max_score_;
}

}  // namespace pos

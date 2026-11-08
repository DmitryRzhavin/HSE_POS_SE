#pragma once
/// Рубрикатор — корень агрегата: набор критериев с уникальными идентификаторами.

#include <vector>

#include "pos/domain/criterion.hpp"

namespace pos {

class Rubric {
 public:
  void add_criterion(const Criterion& criterion);

  const Criterion* find(const CriterionId& id) const;
  const Criterion& get(const CriterionId& id) const;
  const std::vector<Criterion>& criteria() const { return criteria_; }
  bool empty() const { return criteria_.empty(); }
  double total_weight() const;

 private:
  std::vector<Criterion> criteria_;
};

}  // namespace pos

#pragma once
/// Критерий — Entity внутри агрегата Rubric: название, максимум и вес в итоге.

#include <string>

#include "pos/domain/ids.hpp"

namespace pos {

class Criterion {
 public:
  Criterion(const CriterionId& id, const std::string& title, double max_score, double weight);

  const CriterionId& id() const { return id_; }
  const std::string& title() const { return title_; }
  double max_score() const { return max_score_; }
  double weight() const { return weight_; }

  bool accepts(double score) const;
  /// Доля набранного от максимума, 0..1.
  double normalized(double score) const { return score / max_score_; }

 private:
  CriterionId id_;
  std::string title_;
  double max_score_;
  double weight_;
};

}  // namespace pos

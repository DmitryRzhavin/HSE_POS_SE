#pragma once
/// Балл — Value Object: существует только в границах своего критерия.

#include "pos/domain/criterion.hpp"

namespace pos {

class Score {
 public:
  static Score of(double value, const Criterion& criterion);

  double value() const { return value_; }

 private:
  explicit Score(double value) : value_(value) {}

  double value_;
};

}  // namespace pos

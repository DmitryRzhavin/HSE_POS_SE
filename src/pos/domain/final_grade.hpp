#pragma once
/// Итог по студенту — Value Object, вычисленный из оценок и рубрикатора.

#include <cstddef>

#include "pos/domain/ids.hpp"

namespace pos {

struct FinalGrade {
  StudentId student_id;
  double percent;
  std::size_t evaluated_count;
  std::size_t criteria_count;

  /// Итог по десятибалльной шкале.
  double ten_point() const { return percent / 10.0; }
};

}  // namespace pos

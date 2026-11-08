#include "pos/domain/grading_policy.hpp"

#include <cmath>

#include "pos/domain/errors.hpp"

namespace pos {

namespace {

constexpr double kPercentScale = 100.0;

struct WeightedSum {
  double earned = 0;  // сумма вес * доля
  double graded_weight = 0;
};

WeightedSum accumulate(const Rubric& rubric, const std::vector<Evaluation>& evaluations) {
  WeightedSum sum;
  for (const auto& evaluation : evaluations) {
    const Criterion* criterion = rubric.find(evaluation.criterion_id());
    if (criterion == nullptr) {
      continue;
    }
    sum.earned += criterion->weight() * criterion->normalized(evaluation.score().value());
    sum.graded_weight += criterion->weight();
  }
  return sum;
}

}  // namespace

double WeightedAveragePolicy::percent(const Rubric& rubric,
                                      const std::vector<Evaluation>& evaluations) const {
  const double total = rubric.total_weight();
  if (total <= 0) {
    throw DomainError("Рубрикатор пуст: итог посчитать нельзя");
  }
  return accumulate(rubric, evaluations).earned / total * kPercentScale;
}

double GradedOnlyPolicy::percent(const Rubric& rubric,
                                 const std::vector<Evaluation>& evaluations) const {
  if (rubric.empty()) {
    throw DomainError("Рубрикатор пуст: итог посчитать нельзя");
  }
  const WeightedSum sum = accumulate(rubric, evaluations);
  return sum.graded_weight > 0 ? sum.earned / sum.graded_weight * kPercentScale : 0.0;
}

RoundingPolicy::RoundingPolicy(std::shared_ptr<const GradingPolicy> inner, int decimals)
    : inner_(std::move(inner)), decimals_(decimals) {
  if (!inner_) {
    throw DomainError("Для округления нужна базовая политика");
  }
  if (decimals < 0 || decimals > 6) {
    throw DomainError("Число знаков округления должно быть от 0 до 6");
  }
}

double RoundingPolicy::percent(const Rubric& rubric,
                               const std::vector<Evaluation>& evaluations) const {
  const double factor = std::pow(10.0, decimals_);
  return std::round(inner_->percent(rubric, evaluations) * factor) / factor;
}

std::string RoundingPolicy::name() const {
  return inner_->name() + ", округление до " + std::to_string(decimals_) + " зн.";
}

}  // namespace pos

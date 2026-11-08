#pragma once
/// Правила расчёта итога в процентах (0..100).
/// Strategy: сменяемое правило итога. Decorator: RoundingPolicy добавляет округление.

#include <memory>
#include <string>
#include <vector>

#include "pos/domain/evaluation.hpp"
#include "pos/domain/rubric.hpp"

namespace pos {

class GradingPolicy {
 public:
  virtual ~GradingPolicy() = default;
  virtual double percent(const Rubric& rubric, const std::vector<Evaluation>& evaluations) const = 0;
  virtual std::string name() const = 0;
};

/// Невыставленные критерии считаются нулём: вес берётся по всему рубрикатору.
class WeightedAveragePolicy : public GradingPolicy {
 public:
  double percent(const Rubric& rubric, const std::vector<Evaluation>& evaluations) const override;
  std::string name() const override { return "взвешенное среднее по всему рубрикатору"; }
};

/// Учитываются только оценённые критерии: промежуточный итог по уже выставленному.
class GradedOnlyPolicy : public GradingPolicy {
 public:
  double percent(const Rubric& rubric, const std::vector<Evaluation>& evaluations) const override;
  std::string name() const override { return "взвешенное среднее по оценённым критериям"; }
};

class RoundingPolicy : public GradingPolicy {
 public:
  RoundingPolicy(std::shared_ptr<const GradingPolicy> inner, int decimals);

  double percent(const Rubric& rubric, const std::vector<Evaluation>& evaluations) const override;
  std::string name() const override;

 private:
  std::shared_ptr<const GradingPolicy> inner_;
  int decimals_;
};

}  // namespace pos

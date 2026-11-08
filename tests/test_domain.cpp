#include <gtest/gtest.h>

#include <memory>

#include "pos/domain/errors.hpp"
#include "pos/domain/final_grade_calculator.hpp"
#include "pos/domain/gradebook.hpp"
#include "pos/domain/grading_policy.hpp"

namespace pos {
namespace {

Rubric make_rubric() {
  Rubric rubric;
  rubric.add_criterion(Criterion(CriterionId("HW"), "Домашние задания", 10, 3));
  rubric.add_criterion(Criterion(CriterionId("EXAM"), "Экзамен", 40, 1));
  return rubric;
}

Evaluation evaluate(const Rubric& rubric, const char* criterion, double value) {
  return EvaluationFactory::create(rubric, StudentId("S-1"), CriterionId(criterion), value, "",
                                   "2026-01-15");
}

TEST(CriterionTest, RejectsNonPositiveMaxScoreAndWeight) {
  EXPECT_THROW(Criterion(CriterionId("A"), "A", 0, 1), DomainError);
  EXPECT_THROW(Criterion(CriterionId("A"), "A", 10, -1), DomainError);
  EXPECT_THROW(Criterion(CriterionId("A"), " ", 10, 1), DomainError);
}

TEST(RubricTest, RejectsDuplicateCriterion) {
  Rubric rubric = make_rubric();
  EXPECT_THROW(rubric.add_criterion(Criterion(CriterionId("HW"), "Ещё раз", 5, 1)), DomainError);
}

TEST(EvaluationFactoryTest, RejectsScoreAboveMaximum) {
  EXPECT_THROW(evaluate(make_rubric(), "HW", 12), DomainError);
}

TEST(EvaluationFactoryTest, RejectsNegativeScore) {
  EXPECT_THROW(evaluate(make_rubric(), "HW", -0.5), DomainError);
}

TEST(EvaluationFactoryTest, AcceptsBoundaryValues) {
  const Rubric rubric = make_rubric();
  EXPECT_NO_THROW(evaluate(rubric, "HW", 0));
  EXPECT_NO_THROW(evaluate(rubric, "HW", 10));
}

TEST(EvaluationFactoryTest, RejectsCriterionOutsideRubric) {
  EXPECT_THROW(evaluate(make_rubric(), "NOPE", 5), DomainError);
}

TEST(GradebookTest, KeepsSingleEvaluationPerStudentAndCriterion) {
  const Rubric rubric = make_rubric();
  Gradebook gradebook;
  EXPECT_FALSE(gradebook.put(evaluate(rubric, "HW", 5)));
  EXPECT_TRUE(gradebook.put(evaluate(rubric, "HW", 8)));
  ASSERT_EQ(gradebook.all().size(), 1u);
  EXPECT_DOUBLE_EQ(gradebook.find(StudentId("S-1"), CriterionId("HW"))->score().value(), 8);
}

TEST(GradebookTest, CancelRemovesEvaluation) {
  const Rubric rubric = make_rubric();
  Gradebook gradebook;
  gradebook.put(evaluate(rubric, "HW", 5));
  EXPECT_TRUE(gradebook.cancel(StudentId("S-1"), CriterionId("HW")));
  EXPECT_FALSE(gradebook.cancel(StudentId("S-1"), CriterionId("HW")));
  EXPECT_TRUE(gradebook.all().empty());
}

TEST(FinalGradeCalculatorTest, WeightedAverageCountsMissingAsZero) {
  const Rubric rubric = make_rubric();
  Gradebook gradebook;
  gradebook.put(evaluate(rubric, "HW", 10));  // 100% с весом 3 из 4
  FinalGradeCalculator calculator(std::make_shared<WeightedAveragePolicy>());

  const FinalGrade grade = calculator.calculate(rubric, gradebook, StudentId("S-1"));

  EXPECT_DOUBLE_EQ(grade.percent, 75.0);
  EXPECT_DOUBLE_EQ(grade.ten_point(), 7.5);
  EXPECT_EQ(grade.evaluated_count, 1u);
  EXPECT_EQ(grade.criteria_count, 2u);
}

TEST(FinalGradeCalculatorTest, WeightsAffectResult) {
  const Rubric rubric = make_rubric();
  Gradebook gradebook;
  gradebook.put(evaluate(rubric, "HW", 10));
  gradebook.put(evaluate(rubric, "EXAM", 0));
  FinalGradeCalculator calculator(std::make_shared<WeightedAveragePolicy>());
  EXPECT_DOUBLE_EQ(calculator.calculate(rubric, gradebook, StudentId("S-1")).percent, 75.0);
}

TEST(FinalGradeCalculatorTest, StrategyCanBeSwappedToGradedOnly) {
  const Rubric rubric = make_rubric();
  Gradebook gradebook;
  gradebook.put(evaluate(rubric, "EXAM", 20));  // 50% от оценённого
  FinalGradeCalculator calculator(std::make_shared<GradedOnlyPolicy>());
  EXPECT_DOUBLE_EQ(calculator.calculate(rubric, gradebook, StudentId("S-1")).percent, 50.0);
}

TEST(FinalGradeCalculatorTest, EmptyRubricCannotBeGraded) {
  FinalGradeCalculator calculator(std::make_shared<WeightedAveragePolicy>());
  EXPECT_THROW(calculator.calculate(Rubric(), Gradebook(), StudentId("S-1")), DomainError);
}

TEST(RoundingPolicyTest, DecoratesBasePolicy) {
  Rubric rubric;
  rubric.add_criterion(Criterion(CriterionId("A"), "A", 3, 1));
  const std::vector<Evaluation> evaluations{EvaluationFactory::create(
      rubric, StudentId("S-1"), CriterionId("A"), 1, "", "2026-01-15")};  // 33.333...%
  RoundingPolicy rounded(std::make_shared<WeightedAveragePolicy>(), 1);
  EXPECT_DOUBLE_EQ(rounded.percent(rubric, evaluations), 33.3);
}

TEST(RoundingPolicyTest, RejectsMissingInnerPolicy) {
  EXPECT_THROW(RoundingPolicy(nullptr, 2), DomainError);
}

}  // namespace
}  // namespace pos

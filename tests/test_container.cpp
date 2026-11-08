#include <gtest/gtest.h>

#include "test_support.hpp"

namespace pos {
namespace {

TEST(ContainerTest, SharesRepositoriesBetweenServices) {
  Container container;
  auto facade = container.facade();
  facade->add_student("S-1", "Иван");
  EXPECT_EQ(container.student_repository()->all().size(), 1u);
  EXPECT_EQ(container.facade().get(), facade.get());
}

TEST(ContainerTest, ClockOverrideIsUsedByGradingScenario) {
  Container container;
  container.override_clock(std::make_shared<test_support::FixedClock>("2030-02-03"));
  auto facade = test_support::make_course(container);
  facade->set_score("S-1", "HW", 1, "");
  EXPECT_EQ(facade->evaluations(std::nullopt).at(0).date(), "2030-02-03");
}

TEST(ContainerTest, DefaultPolicyIsRoundedWeightedAverage) {
  Container container;
  EXPECT_NE(container.grading_policy()->name().find("округление"), std::string::npos);
}

}  // namespace
}  // namespace pos

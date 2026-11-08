#include <gtest/gtest.h>

#include <sstream>

#include "pos/infrastructure/console_app.hpp"
#include "test_support.hpp"

namespace pos {
namespace {

std::string run_console(Container& container, const std::string& script) {
  std::istringstream input(script);
  std::ostringstream output;
  ConsoleApp(container.facade(), input, output).run();
  return output.str();
}

TEST(ConsoleAppTest, EndToEndScenario) {
  auto writer = std::make_shared<test_support::RecordingWriter>();
  auto container = test_support::make_container(writer);

  const std::string output = run_console(container,
                                         "1\nS-1\nИванов Иван\n"
                                         "2\nHW\nДомашние задания\n10\n1\n"
                                         "5\nS-1\nHW\n9,5\nмолодец\n"
                                         "8\nS-1\n"
                                         "10\nitog.csv\n"
                                         "0\n");

  EXPECT_NE(output.find("Студент Иванов Иван [S-1] добавлен."), std::string::npos);
  EXPECT_NE(output.find("Оценка выставлена."), std::string::npos);
  EXPECT_NE(output.find("95.00% (9.50 из 10)"), std::string::npos);
  EXPECT_NE(output.find("Работа завершена."), std::string::npos);
  EXPECT_EQ(writer->destination_, "itog.csv");
  EXPECT_EQ(writer->rows_.size(), 1u);
}

TEST(ConsoleAppTest, ShowsDomainViolationInsteadOfCrashing) {
  auto container = test_support::make_container();
  test_support::make_course(container);

  const std::string output = run_console(container, "5\nS-1\nHW\n12\n\n5\nS-1\nNOPE\n1\n\n0\n");

  EXPECT_NE(output.find("диапазоне [0, 10]"), std::string::npos);
  EXPECT_NE(output.find("нет в рубрикаторе"), std::string::npos);
  EXPECT_TRUE(container.facade()->evaluations(std::nullopt).empty());
}

TEST(ConsoleAppTest, RepeatsPromptOnInvalidMenuChoice) {
  auto container = test_support::make_container();
  const std::string output = run_console(container, "abc\n99\n0\n");
  EXPECT_NE(output.find("Введите целое число от 0 до 11."), std::string::npos);
}

TEST(ConsoleAppTest, ExitsGracefullyWhenInputEnds) {
  auto container = test_support::make_container();
  const std::string output = run_console(container, "3\n");
  EXPECT_NE(output.find("Ввод завершён"), std::string::npos);
}

}  // namespace
}  // namespace pos

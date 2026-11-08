#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "pos/domain/errors.hpp"
#include "pos/infrastructure/csv_report_writer.hpp"
#include "test_support.hpp"

namespace pos {
namespace {

using test_support::make_container;
using test_support::make_course;
using test_support::RecordingWriter;

TEST(RosterTest, RejectsDuplicateStudent) {
  auto container = make_container();
  auto facade = make_course(container);
  EXPECT_THROW(facade->add_student("S-1", "Другой Иван"), DomainError);
  EXPECT_EQ(facade->students().size(), 1u);
}

TEST(RubricServiceTest, RejectsDuplicateCriterion) {
  auto container = make_container();
  auto facade = make_course(container);
  EXPECT_THROW(facade->add_criterion("HW", "Ещё раз", 5, 1), DomainError);
  EXPECT_EQ(facade->criteria().size(), 2u);
}

TEST(GradingServiceTest, UnknownStudentIsRejected) {
  auto container = make_container();
  auto facade = make_course(container);
  EXPECT_THROW(facade->set_score("S-404", "HW", 5, ""), DomainError);
}

TEST(GradingServiceTest, InvalidScoreIsNotStored) {
  auto container = make_container();
  auto facade = make_course(container);
  EXPECT_THROW(facade->set_score("S-1", "HW", 12, ""), DomainError);
  EXPECT_THROW(facade->set_score("S-1", "NOPE", 1, ""), DomainError);
  EXPECT_TRUE(facade->evaluations(std::nullopt).empty());
}

TEST(GradingServiceTest, SecondAssignmentCorrectsEvaluationAndUsesClockDate) {
  auto container = make_container();
  auto facade = make_course(container);

  EXPECT_FALSE(facade->set_score("S-1", "HW", 5, "").replaced);
  EXPECT_TRUE(facade->set_score("S-1", "HW", 9, "после пересдачи").replaced);

  const auto evaluations = facade->evaluations(std::string("S-1"));
  ASSERT_EQ(evaluations.size(), 1u);
  EXPECT_DOUBLE_EQ(evaluations[0].score().value(), 9);
  EXPECT_EQ(evaluations[0].date(), "2026-01-15");
  EXPECT_EQ(evaluations[0].comment(), "после пересдачи");
}

TEST(GradingServiceTest, CancelRemovesEvaluationFromFinalGrade) {
  auto container = make_container();
  auto facade = make_course(container);
  facade->set_score("S-1", "HW", 10, "");
  EXPECT_DOUBLE_EQ(facade->final_grade("S-1").percent, 75.0);

  EXPECT_TRUE(facade->cancel_score("S-1", "HW"));

  EXPECT_DOUBLE_EQ(facade->final_grade("S-1").percent, 0.0);
  EXPECT_FALSE(facade->cancel_score("S-1", "HW"));
}

TEST(ReportingServiceTest, ExportUsesWriterPort) {
  auto writer = std::make_shared<RecordingWriter>();
  auto container = make_container(writer);
  auto facade = make_course(container);
  facade->set_score("S-1", "EXAM", 40, "");

  EXPECT_EQ(facade->export_report("out.csv"), 1u);

  EXPECT_EQ(writer->destination_, "out.csv");
  ASSERT_EQ(writer->rows_.size(), 1u);
  EXPECT_EQ(writer->rows_[0].student_id, "S-1");
  EXPECT_DOUBLE_EQ(writer->rows_[0].percent, 25.0);
}

TEST(ReportingServiceTest, PolicyCanBeOverriddenInContainer) {
  auto container = make_container();
  container.override_grading_policy(std::make_shared<GradedOnlyPolicy>());
  auto facade = make_course(container);
  facade->set_score("S-1", "EXAM", 40, "");
  EXPECT_DOUBLE_EQ(facade->final_grade("S-1").percent, 100.0);
}

TEST(CsvReportWriterTest, FormatsAndEscapesFields) {
  const std::string csv = CsvReportWriter::to_csv(
      {GradeReportRow{"S-1", "Иванов, Иван", 82.5, 8.25, 2, 3}});
  EXPECT_EQ(csv,
            "student_id,full_name,percent,ten_point,evaluated,criteria\n"
            "S-1,\"Иванов, Иван\",82.50,8.25,2,3\n");
}

TEST(CsvReportWriterTest, WritesFile) {
  const auto path = (std::filesystem::temp_directory_path() / "pos_test_report.csv").string();
  CsvReportWriter().write(path, {GradeReportRow{"S-1", "Иван", 50, 5, 1, 2}});
  std::ifstream file(path);
  std::stringstream content;
  content << file.rdbuf();
  EXPECT_NE(content.str().find("S-1,Иван,50.00,5.00,1,2"), std::string::npos);
  std::remove(path.c_str());
}

TEST(CsvReportWriterTest, ThrowsWhenFileCannotBeOpened) {
  EXPECT_THROW(CsvReportWriter().write("/nonexistent-dir/x.csv", {}), std::runtime_error);
}

TEST(SheetImporterTest, CsvImportsValidRowsAndReportsErrors) {
  auto container = make_container();
  auto facade = make_course(container);
  std::istringstream sheet(
      "student_id,criterion_id,score,comment\n"
      "S-1,HW,8.5,ok\n"
      "\n"
      "S-1,EXAM,99,слишком много\n"
      "S-1,NOPE,1,нет критерия\n"
      "S-1,HW,abc,не число\n"
      "S-1,HW\n");

  const ImportReport report = facade->import_sheet("csv", sheet);

  EXPECT_EQ(report.imported, 1u);
  EXPECT_EQ(report.errors.size(), 4u);
  EXPECT_DOUBLE_EQ(facade->evaluations(std::string("S-1")).at(0).score().value(), 8.5);
}

TEST(SheetImporterTest, ExcelFormatUsesSemicolonAndDecimalComma) {
  auto container = make_container();
  auto facade = make_course(container);
  std::istringstream sheet("student;criterion;score;comment\nS-1;HW;7,5;ок\n");

  const ImportReport report = facade->import_sheet("excel", sheet);

  EXPECT_EQ(report.imported, 1u);
  EXPECT_TRUE(report.errors.empty());
  EXPECT_DOUBLE_EQ(facade->evaluations(std::string("S-1")).at(0).score().value(), 7.5);
}

TEST(SheetImporterTest, UnknownFormatIsRejected) {
  auto container = make_container();
  auto facade = make_course(container);
  std::istringstream sheet("");
  EXPECT_THROW(facade->import_sheet("xml", sheet), std::invalid_argument);
}

}  // namespace
}  // namespace pos

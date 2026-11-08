#include "pos/container.hpp"

#include "pos/infrastructure/csv_report_writer.hpp"
#include "pos/infrastructure/in_memory_repositories.hpp"
#include "pos/infrastructure/system_clock.hpp"

namespace pos {

namespace {
constexpr int kDefaultRoundingDecimals = 2;
}

std::shared_ptr<StudentRepository> Container::student_repository() {
  if (!student_repository_) {
    student_repository_ = std::make_shared<InMemoryStudentRepository>();
  }
  return student_repository_;
}

std::shared_ptr<RubricRepository> Container::rubric_repository() {
  if (!rubric_repository_) {
    rubric_repository_ = std::make_shared<InMemoryRubricRepository>();
  }
  return rubric_repository_;
}

std::shared_ptr<GradebookRepository> Container::gradebook_repository() {
  if (!gradebook_repository_) {
    gradebook_repository_ = std::make_shared<InMemoryGradebookRepository>();
  }
  return gradebook_repository_;
}

std::shared_ptr<Clock> Container::clock() {
  if (!clock_) {
    clock_ = std::make_shared<SystemClock>();
  }
  return clock_;
}

std::shared_ptr<GradeReportWriter> Container::report_writer() {
  if (!report_writer_) {
    report_writer_ = std::make_shared<CsvReportWriter>();
  }
  return report_writer_;
}

std::shared_ptr<const GradingPolicy> Container::grading_policy() {
  if (!grading_policy_) {
    grading_policy_ = std::make_shared<RoundingPolicy>(std::make_shared<WeightedAveragePolicy>(),
                                                       kDefaultRoundingDecimals);
  }
  return grading_policy_;
}

Container& Container::override_clock(std::shared_ptr<Clock> clock) {
  clock_ = std::move(clock);
  facade_.reset();
  return *this;
}

Container& Container::override_report_writer(std::shared_ptr<GradeReportWriter> writer) {
  report_writer_ = std::move(writer);
  facade_.reset();
  return *this;
}

Container& Container::override_grading_policy(std::shared_ptr<const GradingPolicy> policy) {
  grading_policy_ = std::move(policy);
  facade_.reset();
  return *this;
}

std::shared_ptr<PosFacade> Container::facade() {
  if (!facade_) {
    auto students = student_repository();
    auto rubric = rubric_repository();
    auto gradebook = gradebook_repository();
    auto grading = std::make_shared<GradingService>(students, rubric, gradebook, clock());
    auto calculator = std::make_shared<FinalGradeCalculator>(grading_policy());
    auto reporting = std::make_shared<ReportingService>(students, rubric, gradebook, calculator,
                                                        report_writer());
    PosFacade::ImporterMap importers{
        {"csv", std::make_shared<CsvSheetImporter>(grading)},
        {"excel", std::make_shared<ExcelCsvSheetImporter>(grading)},
    };
    facade_ = std::make_shared<PosFacade>(std::make_shared<RosterService>(students),
                                          std::make_shared<RubricService>(rubric), grading,
                                          reporting, std::move(importers));
  }
  return facade_;
}

}  // namespace pos

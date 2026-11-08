#pragma once
/// Сценарии итогов и выгрузки.

#include <memory>
#include <string>
#include <vector>

#include "pos/application/ports.hpp"
#include "pos/domain/final_grade_calculator.hpp"

namespace pos {

class ReportingService {
 public:
  ReportingService(std::shared_ptr<StudentRepository> students,
                   std::shared_ptr<RubricRepository> rubric,
                   std::shared_ptr<GradebookRepository> gradebook,
                   std::shared_ptr<FinalGradeCalculator> calculator,
                   std::shared_ptr<GradeReportWriter> writer);

  FinalGrade final_grade(const std::string& student_id) const;
  std::vector<GradeReportRow> report() const;
  /// Возвращает число выгруженных строк.
  std::size_t export_report(const std::string& destination);
  std::string policy_name() const { return calculator_->policy().name(); }

 private:
  std::shared_ptr<StudentRepository> students_;
  std::shared_ptr<RubricRepository> rubric_;
  std::shared_ptr<GradebookRepository> gradebook_;
  std::shared_ptr<FinalGradeCalculator> calculator_;
  std::shared_ptr<GradeReportWriter> writer_;
};

}  // namespace pos

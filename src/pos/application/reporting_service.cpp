#include "pos/application/reporting_service.hpp"

#include "pos/domain/errors.hpp"

namespace pos {

ReportingService::ReportingService(std::shared_ptr<StudentRepository> students,
                                   std::shared_ptr<RubricRepository> rubric,
                                   std::shared_ptr<GradebookRepository> gradebook,
                                   std::shared_ptr<FinalGradeCalculator> calculator,
                                   std::shared_ptr<GradeReportWriter> writer)
    : students_(std::move(students)), rubric_(std::move(rubric)),
      gradebook_(std::move(gradebook)), calculator_(std::move(calculator)),
      writer_(std::move(writer)) {}

FinalGrade ReportingService::final_grade(const std::string& student_id) const {
  const StudentId id(student_id);
  if (!students_->find(id)) {
    throw DomainError("Студента '" + id.value() + "' нет в списке");
  }
  return calculator_->calculate(rubric_->load(), gradebook_->load(), id);
}

std::vector<GradeReportRow> ReportingService::report() const {
  const Rubric rubric = rubric_->load();
  const Gradebook gradebook = gradebook_->load();
  std::vector<GradeReportRow> rows;
  for (const auto& student : students_->all()) {
    const FinalGrade grade = calculator_->calculate(rubric, gradebook, student.id());
    rows.push_back(GradeReportRow{student.id().value(), student.full_name(), grade.percent,
                                  grade.ten_point(), grade.evaluated_count,
                                  grade.criteria_count});
  }
  return rows;
}

std::size_t ReportingService::export_report(const std::string& destination) {
  const std::vector<GradeReportRow> rows = report();
  writer_->write(destination, rows);
  return rows.size();
}

}  // namespace pos

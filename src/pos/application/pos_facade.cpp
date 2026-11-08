#include "pos/application/pos_facade.hpp"

#include <stdexcept>

namespace pos {

PosFacade::PosFacade(std::shared_ptr<RosterService> roster, std::shared_ptr<RubricService> rubric,
                     std::shared_ptr<GradingService> grading,
                     std::shared_ptr<ReportingService> reporting, ImporterMap importers)
    : roster_(std::move(roster)), rubric_(std::move(rubric)), grading_(std::move(grading)),
      reporting_(std::move(reporting)), importers_(std::move(importers)) {}

Student PosFacade::add_student(const std::string& id, const std::string& full_name) {
  return roster_->register_student(id, full_name);
}

std::vector<Student> PosFacade::students() const { return roster_->list_students(); }

Criterion PosFacade::add_criterion(const std::string& id, const std::string& title,
                                   double max_score, double weight) {
  return rubric_->define_criterion(id, title, max_score, weight);
}

std::vector<Criterion> PosFacade::criteria() const { return rubric_->list_criteria(); }

AssignmentResult PosFacade::set_score(const std::string& student_id,
                                      const std::string& criterion_id, double value,
                                      const std::string& comment) {
  return grading_->assign_score(student_id, criterion_id, value, comment);
}

bool PosFacade::cancel_score(const std::string& student_id, const std::string& criterion_id) {
  return grading_->cancel_score(student_id, criterion_id);
}

std::vector<Evaluation> PosFacade::evaluations(const std::optional<std::string>& student_id) const {
  return grading_->list_evaluations(student_id);
}

FinalGrade PosFacade::final_grade(const std::string& student_id) const {
  return reporting_->final_grade(student_id);
}

std::vector<GradeReportRow> PosFacade::report() const { return reporting_->report(); }

std::size_t PosFacade::export_report(const std::string& destination) {
  return reporting_->export_report(destination);
}

std::string PosFacade::policy_name() const { return reporting_->policy_name(); }

std::vector<std::string> PosFacade::importer_formats() const {
  std::vector<std::string> formats;
  for (const auto& entry : importers_) {
    formats.push_back(entry.first);
  }
  return formats;
}

ImportReport PosFacade::import_sheet(const std::string& format, std::istream& input) {
  auto it = importers_.find(format);
  if (it == importers_.end()) {
    throw std::invalid_argument("неизвестный формат импорта: " + format);
  }
  return it->second->import_sheet(input);
}

}  // namespace pos

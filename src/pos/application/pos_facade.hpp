#pragma once
/// Facade: единая точка сценариев для консоли. UI не знает о репозиториях,
/// фабриках и политиках — только об этом классе.

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "pos/application/grading_service.hpp"
#include "pos/application/reporting_service.hpp"
#include "pos/application/roster_service.hpp"
#include "pos/application/rubric_service.hpp"
#include "pos/application/sheet_importer.hpp"

namespace pos {

class PosFacade {
 public:
  using ImporterMap = std::map<std::string, std::shared_ptr<SheetImporter>>;

  PosFacade(std::shared_ptr<RosterService> roster, std::shared_ptr<RubricService> rubric,
            std::shared_ptr<GradingService> grading, std::shared_ptr<ReportingService> reporting,
            ImporterMap importers);

  Student add_student(const std::string& id, const std::string& full_name);
  std::vector<Student> students() const;

  Criterion add_criterion(const std::string& id, const std::string& title, double max_score,
                          double weight);
  std::vector<Criterion> criteria() const;

  AssignmentResult set_score(const std::string& student_id, const std::string& criterion_id,
                             double value, const std::string& comment);
  bool cancel_score(const std::string& student_id, const std::string& criterion_id);
  std::vector<Evaluation> evaluations(const std::optional<std::string>& student_id) const;

  FinalGrade final_grade(const std::string& student_id) const;
  std::vector<GradeReportRow> report() const;
  std::size_t export_report(const std::string& destination);
  std::string policy_name() const;

  std::vector<std::string> importer_formats() const;
  /// Бросает std::invalid_argument для неизвестного формата.
  ImportReport import_sheet(const std::string& format, std::istream& input);

 private:
  std::shared_ptr<RosterService> roster_;
  std::shared_ptr<RubricService> rubric_;
  std::shared_ptr<GradingService> grading_;
  std::shared_ptr<ReportingService> reporting_;
  ImporterMap importers_;
};

}  // namespace pos

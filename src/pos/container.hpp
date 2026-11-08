#pragma once
/// Composition root: единственное место, где конкретные классы связываются
/// с портами. Зависимости создаются лениво и переиспользуются (singleton на контейнер).
/// Для тестов любую зависимость можно подменить через override_*().

#include <memory>

#include "pos/application/pos_facade.hpp"
#include "pos/domain/grading_policy.hpp"

namespace pos {

class Container {
 public:
  std::shared_ptr<PosFacade> facade();

  std::shared_ptr<StudentRepository> student_repository();
  std::shared_ptr<RubricRepository> rubric_repository();
  std::shared_ptr<GradebookRepository> gradebook_repository();
  std::shared_ptr<Clock> clock();
  std::shared_ptr<GradeReportWriter> report_writer();
  std::shared_ptr<const GradingPolicy> grading_policy();

  Container& override_clock(std::shared_ptr<Clock> clock);
  Container& override_report_writer(std::shared_ptr<GradeReportWriter> writer);
  Container& override_grading_policy(std::shared_ptr<const GradingPolicy> policy);

 private:
  std::shared_ptr<StudentRepository> student_repository_;
  std::shared_ptr<RubricRepository> rubric_repository_;
  std::shared_ptr<GradebookRepository> gradebook_repository_;
  std::shared_ptr<Clock> clock_;
  std::shared_ptr<GradeReportWriter> report_writer_;
  std::shared_ptr<const GradingPolicy> grading_policy_;
  std::shared_ptr<PosFacade> facade_;
};

}  // namespace pos

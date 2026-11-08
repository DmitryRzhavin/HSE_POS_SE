#pragma once
/// Общие фейки и помощники для тестов.

#include <memory>
#include <string>
#include <vector>

#include "pos/application/ports.hpp"
#include "pos/container.hpp"

namespace pos::test_support {

class FixedClock : public Clock {
 public:
  explicit FixedClock(std::string date = "2026-01-15") : date_(std::move(date)) {}
  std::string today() const override { return date_; }

 private:
  std::string date_;
};

class RecordingWriter : public GradeReportWriter {
 public:
  void write(const std::string& destination, const std::vector<GradeReportRow>& rows) override {
    destination_ = destination;
    rows_ = rows;
  }

  std::string destination_;
  std::vector<GradeReportRow> rows_;
};

/// Контейнер с фиксированной датой и фейковым экспортом; рубрикатор и студенты не заданы.
inline Container make_container(std::shared_ptr<RecordingWriter> writer = nullptr) {
  Container container;
  container.override_clock(std::make_shared<FixedClock>());
  if (writer) {
    container.override_report_writer(std::move(writer));
  }
  return container;
}

/// Две оценки с весами 3 и 1: HW (макс. 10, вес 3), EXAM (макс. 40, вес 1); студент S-1.
inline std::shared_ptr<PosFacade> make_course(Container& container) {
  auto facade = container.facade();
  facade->add_student("S-1", "Иванов Иван");
  facade->add_criterion("HW", "Домашние задания", 10, 3);
  facade->add_criterion("EXAM", "Экзамен", 40, 1);
  return facade;
}

}  // namespace pos::test_support

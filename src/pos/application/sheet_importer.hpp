#pragma once
/// Template Method: общий каркас импорта ведомости, разный разбор строк.
/// Формат: заголовок, затем строки «студент, критерий, балл, комментарий».

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "pos/application/grading_service.hpp"

namespace pos {

struct ParsedRow {
  std::string student_id;
  std::string criterion_id;
  double score;
  std::string comment;
};

struct ImportReport {
  std::size_t imported = 0;
  std::vector<std::string> errors;
};

class SheetImporter {
 public:
  explicit SheetImporter(std::shared_ptr<GradingService> grading);
  virtual ~SheetImporter() = default;

  /// Каркас алгоритма: чтение строк, разбор, применение, сбор ошибок.
  ImportReport import_sheet(std::istream& input) const;

 protected:
  /// Разбор одной строки; при ошибке формата бросает std::invalid_argument.
  virtual ParsedRow parse_line(const std::string& line) const = 0;

  static std::vector<std::string> split(const std::string& line, char delimiter);
  static double to_number(const std::string& text, bool decimal_comma);

 private:
  std::shared_ptr<GradingService> grading_;
};

/// Стандартный CSV: разделитель «,», десятичная точка.
class CsvSheetImporter : public SheetImporter {
 public:
  using SheetImporter::SheetImporter;

 protected:
  ParsedRow parse_line(const std::string& line) const override;
};

/// CSV из русской локали Excel: разделитель «;», десятичная запятая.
class ExcelCsvSheetImporter : public SheetImporter {
 public:
  using SheetImporter::SheetImporter;

 protected:
  ParsedRow parse_line(const std::string& line) const override;
};

}  // namespace pos

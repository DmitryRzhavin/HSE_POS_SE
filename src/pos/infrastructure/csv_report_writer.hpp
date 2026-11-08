#pragma once
/// Адаптер выгрузки итогов в CSV-файл.

#include "pos/application/ports.hpp"

namespace pos {

class CsvReportWriter : public GradeReportWriter {
 public:
  void write(const std::string& destination, const std::vector<GradeReportRow>& rows) override;

  /// Формирование текста отделено от записи файла — так проще тестировать.
  static std::string to_csv(const std::vector<GradeReportRow>& rows);
};

}  // namespace pos

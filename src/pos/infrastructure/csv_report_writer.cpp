#include "pos/infrastructure/csv_report_writer.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace pos {

namespace {

std::string escape(const std::string& field) {
  if (field.find_first_of(",\"\n") == std::string::npos) {
    return field;
  }
  std::string result = "\"";
  for (const char symbol : field) {
    if (symbol == '"') {
      result += '"';
    }
    result += symbol;
  }
  return result + "\"";
}

}  // namespace

std::string CsvReportWriter::to_csv(const std::vector<GradeReportRow>& rows) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(2);
  out << "student_id,full_name,percent,ten_point,evaluated,criteria\n";
  for (const auto& row : rows) {
    out << escape(row.student_id) << ',' << escape(row.full_name) << ',' << row.percent << ','
        << row.ten_point << ',' << row.evaluated_count << ',' << row.criteria_count << '\n';
  }
  return out.str();
}

void CsvReportWriter::write(const std::string& destination,
                            const std::vector<GradeReportRow>& rows) {
  std::ofstream file(destination);
  if (!file) {
    throw std::runtime_error("не удалось открыть файл для записи: " + destination);
  }
  file << to_csv(rows);
  if (!file) {
    throw std::runtime_error("ошибка записи в файл: " + destination);
  }
}

}  // namespace pos

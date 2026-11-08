#include "pos/application/sheet_importer.hpp"

#include <cstddef>
#include <sstream>
#include <stdexcept>

namespace pos {

namespace {

constexpr std::size_t kMinFields = 3;
constexpr std::size_t kMaxFields = 4;

std::string trimmed(const std::string& text) {
  const auto begin = text.find_first_not_of(" \t\r");
  if (begin == std::string::npos) {
    return "";
  }
  return text.substr(begin, text.find_last_not_of(" \t\r") - begin + 1);
}

ParsedRow to_row(const std::vector<std::string>& fields, bool decimal_comma,
                 double (*convert)(const std::string&, bool)) {
  if (fields.size() < kMinFields || fields.size() > kMaxFields) {
    throw std::invalid_argument("ожидалось 3–4 поля: студент, критерий, балл[, комментарий]");
  }
  return ParsedRow{trimmed(fields[0]), trimmed(fields[1]), convert(fields[2], decimal_comma),
                   fields.size() == kMaxFields ? trimmed(fields[3]) : ""};
}

}  // namespace

SheetImporter::SheetImporter(std::shared_ptr<GradingService> grading)
    : grading_(std::move(grading)) {}

ImportReport SheetImporter::import_sheet(std::istream& input) const {
  ImportReport report;
  std::string line;
  std::size_t line_number = 0;
  while (std::getline(input, line)) {
    ++line_number;
    if (line_number == 1 || trimmed(line).empty()) {
      continue;  // заголовок и пустые строки
    }
    try {
      const ParsedRow row = parse_line(line);
      grading_->assign_score(row.student_id, row.criterion_id, row.score, row.comment);
      ++report.imported;
    } catch (const std::exception& error) {
      report.errors.push_back("строка " + std::to_string(line_number) + ": " + error.what());
    }
  }
  return report;
}

std::vector<std::string> SheetImporter::split(const std::string& line, char delimiter) {
  std::vector<std::string> fields;
  std::stringstream stream(line);
  std::string field;
  while (std::getline(stream, field, delimiter)) {
    fields.push_back(field);
  }
  if (!line.empty() && line.back() == delimiter) {
    fields.emplace_back();
  }
  return fields;
}

double SheetImporter::to_number(const std::string& text, bool decimal_comma) {
  std::string normalized = trimmed(text);
  if (decimal_comma) {
    for (char& symbol : normalized) {
      if (symbol == ',') {
        symbol = '.';
      }
    }
  }
  std::size_t parsed = 0;
  double value = 0;
  try {
    value = std::stod(normalized, &parsed);
  } catch (const std::exception&) {
    throw std::invalid_argument("'" + text + "' не является числом");
  }
  if (parsed != normalized.size()) {
    throw std::invalid_argument("'" + text + "' не является числом");
  }
  return value;
}

ParsedRow CsvSheetImporter::parse_line(const std::string& line) const {
  return to_row(split(line, ','), false, &SheetImporter::to_number);
}

ParsedRow ExcelCsvSheetImporter::parse_line(const std::string& line) const {
  return to_row(split(line, ';'), true, &SheetImporter::to_number);
}

}  // namespace pos

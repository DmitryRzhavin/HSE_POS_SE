#pragma once
/// Общая нормализация текстовых полей домена.

#include <string>

namespace pos {

/// Обрезает пробелы по краям; пустое значение нарушает инвариант.
std::string normalize_text(const std::string& value, const std::string& field_name);

}  // namespace pos

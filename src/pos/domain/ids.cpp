#include "pos/domain/ids.hpp"

#include "pos/domain/text.hpp"

namespace pos {

StudentId::StudentId(const std::string& value)
    : value_(normalize_text(value, "Идентификатор студента")) {}

CriterionId::CriterionId(const std::string& value)
    : value_(normalize_text(value, "Идентификатор критерия")) {}

}  // namespace pos

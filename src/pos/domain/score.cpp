#include "pos/domain/score.hpp"

#include <sstream>

#include "pos/domain/errors.hpp"

namespace pos {

Score Score::of(double value, const Criterion& criterion) {
  if (!criterion.accepts(value)) {
    std::ostringstream message;
    message << "Балл должен быть в диапазоне [0, " << criterion.max_score()
            << "] для критерия '" << criterion.id().value() << "'";
    throw DomainError(message.str());
  }
  return Score(value);
}

}  // namespace pos

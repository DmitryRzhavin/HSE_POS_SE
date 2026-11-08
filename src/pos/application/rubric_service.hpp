#pragma once
/// Сценарии с рубрикатором.

#include <memory>
#include <string>
#include <vector>

#include "pos/application/ports.hpp"

namespace pos {

class RubricService {
 public:
  explicit RubricService(std::shared_ptr<RubricRepository> rubric);

  Criterion define_criterion(const std::string& id, const std::string& title, double max_score,
                             double weight);
  std::vector<Criterion> list_criteria() const;

 private:
  std::shared_ptr<RubricRepository> rubric_;
};

}  // namespace pos

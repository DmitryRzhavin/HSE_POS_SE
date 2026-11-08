#pragma once
/// Интерактивный консольный интерфейс. Знает только PosFacade;
/// бизнес-правила остаются в domain/ и application/.

#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>

#include "pos/application/pos_facade.hpp"

namespace pos {

class ConsoleApp {
 public:
  ConsoleApp(std::shared_ptr<PosFacade> facade, std::istream& input, std::ostream& output);

  void run();

 private:
  struct InputEnded {};

  std::string read_line(const std::string& prompt);
  int ask_int(const std::string& prompt, int minimum, int maximum);
  double ask_double(const std::string& prompt, double minimum);

  void add_student();
  void add_criterion();
  void show_students();
  void show_criteria();
  void set_score();
  void cancel_score();
  void show_evaluations();
  void show_final_grade();
  void show_report();
  void export_report();
  void import_sheet();

  void print_menu();

  std::shared_ptr<PosFacade> facade_;
  std::istream& input_;
  std::ostream& output_;
};

}  // namespace pos

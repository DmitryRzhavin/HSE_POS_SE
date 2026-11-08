#include "pos/infrastructure/console_app.hpp"

#include <fstream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>

#include "pos/domain/errors.hpp"

namespace pos {

namespace {

constexpr int kExitChoice = 0;
constexpr int kMenuMax = 11;

std::string join(const std::vector<std::string>& items) {
  std::string result;
  for (const auto& item : items) {
    result += (result.empty() ? "" : "/") + item;
  }
  return result;
}

}  // namespace

ConsoleApp::ConsoleApp(std::shared_ptr<PosFacade> facade, std::istream& input,
                       std::ostream& output)
    : facade_(std::move(facade)), input_(input), output_(output) {
  output_ << std::fixed << std::setprecision(2);
}

std::string ConsoleApp::read_line(const std::string& prompt) {
  output_ << prompt << std::flush;
  std::string line;
  if (!std::getline(input_, line)) {
    throw InputEnded{};
  }
  const auto begin = line.find_first_not_of(" \t\r");
  if (begin == std::string::npos) {
    return "";
  }
  return line.substr(begin, line.find_last_not_of(" \t\r") - begin + 1);
}

int ConsoleApp::ask_int(const std::string& prompt, int minimum, int maximum) {
  while (true) {
    const std::string raw = read_line(prompt);
    try {
      std::size_t parsed = 0;
      const int value = std::stoi(raw, &parsed);
      if (parsed == raw.size() && value >= minimum && value <= maximum) {
        return value;
      }
    } catch (const std::exception&) {
    }
    output_ << "Введите целое число от " << minimum << " до " << maximum << ".\n";
  }
}

double ConsoleApp::ask_double(const std::string& prompt, double minimum) {
  while (true) {
    std::string raw = read_line(prompt);
    for (char& symbol : raw) {
      if (symbol == ',') {
        symbol = '.';
      }
    }
    try {
      std::size_t parsed = 0;
      const double value = std::stod(raw, &parsed);
      if (parsed == raw.size() && value >= minimum) {
        return value;
      }
    } catch (const std::exception&) {
    }
    output_ << "Введите число не меньше " << minimum << ".\n";
  }
}

void ConsoleApp::print_menu() {
  output_ << "\n1 — добавить студента | 2 — добавить критерий | 3 — студенты | 4 — критерии\n"
          << "5 — выставить/исправить оценку | 6 — отменить оценку | 7 — оценки\n"
          << "8 — итог студента | 9 — итоги всех | 10 — экспорт итогов в CSV"
          << " | 11 — импорт ведомости | 0 — выход\n";
}

void ConsoleApp::add_student() {
  const std::string id = read_line("Идентификатор студента: ");
  const std::string name = read_line("ФИО: ");
  const Student student = facade_->add_student(id, name);
  output_ << "Студент " << student.full_name() << " [" << student.id().value() << "] добавлен.\n";
}

void ConsoleApp::add_criterion() {
  const std::string id = read_line("Идентификатор критерия: ");
  const std::string title = read_line("Название: ");
  const double max_score = ask_double("Максимальный балл: ", 0.0);
  const double weight = ask_double("Вес в итоге: ", 0.0);
  const Criterion criterion = facade_->add_criterion(id, title, max_score, weight);
  output_ << "Критерий " << criterion.title() << " [" << criterion.id().value()
          << "] добавлен.\n";
}

void ConsoleApp::show_students() {
  const auto students = facade_->students();
  output_ << "Студенты:\n";
  if (students.empty()) {
    output_ << "  Список пока пуст.\n";
  }
  for (const auto& student : students) {
    output_ << "  " << student.full_name() << " [" << student.id().value() << "]\n";
  }
}

void ConsoleApp::show_criteria() {
  const auto criteria = facade_->criteria();
  output_ << "Рубрикатор:\n";
  if (criteria.empty()) {
    output_ << "  Критериев пока нет.\n";
  }
  for (const auto& criterion : criteria) {
    output_ << "  " << criterion.title() << " [" << criterion.id().value() << "], максимум "
            << criterion.max_score() << ", вес " << criterion.weight() << "\n";
  }
}

void ConsoleApp::set_score() {
  const std::string student = read_line("Идентификатор студента: ");
  const std::string criterion = read_line("Идентификатор критерия: ");
  const double value = ask_double("Балл: ", 0.0);
  const std::string comment = read_line("Комментарий (можно пустой): ");
  const AssignmentResult result = facade_->set_score(student, criterion, value, comment);
  output_ << (result.replaced ? "Оценка исправлена.\n" : "Оценка выставлена.\n");
}

void ConsoleApp::cancel_score() {
  const std::string student = read_line("Идентификатор студента: ");
  const std::string criterion = read_line("Идентификатор критерия: ");
  output_ << (facade_->cancel_score(student, criterion) ? "Оценка отменена.\n"
                                                         : "Такой оценки нет.\n");
}

void ConsoleApp::show_evaluations() {
  const std::string student = read_line("Студент (пусто — все): ");
  const auto evaluations = facade_->evaluations(
      student.empty() ? std::nullopt : std::optional<std::string>(student));
  output_ << "Оценки:\n";
  if (evaluations.empty()) {
    output_ << "  Оценок пока нет.\n";
  }
  for (const auto& evaluation : evaluations) {
    output_ << "  " << evaluation.student_id().value() << " / "
            << evaluation.criterion_id().value() << ": " << evaluation.score().value() << " ("
            << evaluation.date() << ")";
    if (!evaluation.comment().empty()) {
      output_ << " — " << evaluation.comment();
    }
    output_ << "\n";
  }
}

void ConsoleApp::show_final_grade() {
  const FinalGrade grade = facade_->final_grade(read_line("Идентификатор студента: "));
  output_ << "Итог " << grade.student_id.value() << ": " << grade.percent << "% ("
          << grade.ten_point() << " из 10), оценено критериев " << grade.evaluated_count << " из "
          << grade.criteria_count << "\n";
  output_ << "Правило: " << facade_->policy_name() << "\n";
}

void ConsoleApp::show_report() {
  const auto rows = facade_->report();
  output_ << "Итоги:\n";
  if (rows.empty()) {
    output_ << "  Студентов пока нет.\n";
  }
  for (const auto& row : rows) {
    output_ << "  " << row.full_name << " [" << row.student_id << "]: " << row.percent << "% ("
            << row.ten_point << "/10), критериев " << row.evaluated_count << "/"
            << row.criteria_count << "\n";
  }
}

void ConsoleApp::export_report() {
  const std::string path = read_line("Путь к CSV-файлу: ");
  const std::size_t count = facade_->export_report(path);
  output_ << "Выгружено строк: " << count << " → " << path << "\n";
}

void ConsoleApp::import_sheet() {
  const std::string format = read_line("Формат (" + join(facade_->importer_formats()) + "): ");
  const std::string path = read_line("Путь к файлу ведомости: ");
  std::ifstream file(path);
  if (!file) {
    output_ << "Не удалось открыть файл: " << path << "\n";
    return;
  }
  const ImportReport report = facade_->import_sheet(format, file);
  output_ << "Импортировано оценок: " << report.imported << ", ошибок: " << report.errors.size()
          << "\n";
  for (const auto& error : report.errors) {
    output_ << "  " << error << "\n";
  }
}

void ConsoleApp::run() {
  const std::map<int, std::function<void()>> actions{
      {1, [this] { add_student(); }},     {2, [this] { add_criterion(); }},
      {3, [this] { show_students(); }},   {4, [this] { show_criteria(); }},
      {5, [this] { set_score(); }},       {6, [this] { cancel_score(); }},
      {7, [this] { show_evaluations(); }}, {8, [this] { show_final_grade(); }},
      {9, [this] { show_report(); }},     {10, [this] { export_report(); }},
      {11, [this] { import_sheet(); }},
  };
  output_ << "ПОС — преподавательская оценка студентов\n";
  try {
    while (true) {
      print_menu();
      const int choice = ask_int("Ваш выбор: ", kExitChoice, kMenuMax);
      if (choice == kExitChoice) {
        output_ << "Работа завершена.\n";
        return;
      }
      try {
        actions.at(choice)();
      } catch (const DomainError& error) {
        output_ << "Нарушено правило: " << error.what() << "\n";
      } catch (const std::invalid_argument& error) {
        output_ << "Некорректные данные: " << error.what() << "\n";
      } catch (const std::runtime_error& error) {
        output_ << "Ошибка: " << error.what() << "\n";
      }
    }
  } catch (const InputEnded&) {
    output_ << "\nВвод завершён. Работа завершена.\n";
  }
}

}  // namespace pos

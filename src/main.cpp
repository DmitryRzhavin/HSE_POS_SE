/// Точка входа: собирает граф зависимостей и запускает консольное приложение.

#include <iostream>

#include "pos/container.hpp"
#include "pos/infrastructure/console_app.hpp"

int main() {
  pos::Container container;
  pos::ConsoleApp(container.facade(), std::cin, std::cout).run();
  return 0;
}

cmake_minimum_required(VERSION 3.16)
project(pos VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(POS_BUILD_TESTS "Собирать unit-тесты" ON)

file(GLOB_RECURSE POS_SOURCES CONFIGURE_DEPENDS src/pos/*.cpp)

add_library(pos_core STATIC ${POS_SOURCES})
target_include_directories(pos_core PUBLIC src)
if(MSVC)
  target_compile_options(pos_core PRIVATE /W4 /utf-8)
else()
  target_compile_options(pos_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(pos_app src/main.cpp)
target_link_libraries(pos_app PRIVATE pos_core)

if(POS_BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  file(GLOB POS_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
  add_executable(pos_tests ${POS_TEST_SOURCES})
  target_include_directories(pos_tests PRIVATE tests)
  target_link_libraries(pos_tests PRIVATE pos_core GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(pos_tests)
endif()

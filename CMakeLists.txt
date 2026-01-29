cmake_minimum_required(VERSION 3.16)
project(DatasetInspector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Test)

qt_standard_project_setup()

qt_add_executable(DatasetInspector
  src/main.cpp
  src/MainWindow.h src/MainWindow.cpp
  src/CsvTableModel.h src/CsvTableModel.cpp
  src/CsvLoader.h src/CsvLoader.cpp
)

target_link_libraries(DatasetInspector PRIVATE Qt6::Widgets Qt6::Concurrent)


# ---- Tests ----
enable_testing()

qt_add_executable(DatasetInspectorTests
  tests/test_csv.cpp
  src/CsvTableModel.h src/CsvTableModel.cpp
  src/CsvLoader.h src/CsvLoader.cpp
)

target_link_libraries(DatasetInspectorTests PRIVATE Qt6::Test Qt6::Concurrent Qt6::Widgets)

add_test(NAME DatasetInspectorTests COMMAND DatasetInspectorTests)
cmake_minimum_required(VERSION 3.20)
project(qLang VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для сборки тестов
option(BUILD_TESTS "Build tests" ON)

# Добавляем поддиректорию lib
add_subdirectory(lib)

# Создаем исполняемый файл
add_executable(qlang main.cpp)

# Линкуем библиотеку
target_link_libraries(qlang PRIVATE qlang_lib)

# Устанавливаем include директории
target_include_directories(qlang PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
)
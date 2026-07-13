cmake_minimum_required(VERSION 3.20)

# Собираем все исходники из src
file(GLOB_RECURSE LIB_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

file(GLOB_RECURSE LIB_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx"
)

# Создаем библиотеку
add_library(qlang_lib STATIC ${LIB_SOURCES} ${LIB_HEADERS})

# Устанавливаем include директории для библиотеки
target_include_directories(qlang_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Устанавливаем свойства компиляции
target_compile_features(qlang_lib PUBLIC cxx_std_17)

# Сборка тестов, если опция включена
if(BUILD_TESTS)
    # Подключаем Google Test через FetchContent
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )

    # Для Windows отключаем предупреждения
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    # Собираем тестовые файлы
    file(GLOB_RECURSE TEST_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc"
    )

    # Создаем исполняемый файл для тестов
    add_executable(qlang_test ${TEST_SOURCES})

    # Линкуем библиотеки для тестов
    target_link_libraries(qlang_test PRIVATE
            qlang_lib
            GTest::gtest_main
            GTest::gmock_main
    )

    # Устанавливаем include директории для тестов
    target_include_directories(qlang_test PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/test
    )

    # Включаем тестирование
    enable_testing()
    add_test(NAME qlang_tests COMMAND qlang_test)

    # Дополнительные настройки для Google Test
    target_compile_features(qlang_test PUBLIC cxx_std_17)

    # Отключаем предупреждения от Google Test
    if(MSVC)
        target_compile_options(qlang_test PRIVATE /W0)
    else()
        target_compile_options(qlang_test PRIVATE -w)
    endif()
endif()
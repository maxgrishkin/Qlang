cmake_minimum_required(VERSION 3.14)
project(qlang_lib)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(qlang_tests
        src/Lexer.h
        src/token.h

)

target_include_directories(qlang_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(qlang_tests PRIVATE gtest gtest_main)

enable_testing()
add_test(NAME Algorithms_tests COMMAND Algorithms_tests)
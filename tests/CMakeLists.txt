add_executable(test_main test_main.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(test_main PRIVATE -Wall -Wextra -Wpedantic -Werror -pedantic-errors)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_main PRIVATE /W4 /WX /permissive-)
endif()

add_test(Test_Main test_main)
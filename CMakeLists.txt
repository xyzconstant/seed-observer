cmake_minimum_required(VERSION 3.22)
project(SeedObserver LANGUAGES CXX)

option(BUILD_TESTS "Build test_main." ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
add_executable(seed_observer main.cpp network.cpp netaddress.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(seed_observer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Werror=implicit-fallthrough
        -pedantic-errors 
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(seed_observer PRIVATE /W4 /WX /permissive-)
endif()
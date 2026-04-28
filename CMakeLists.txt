cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
endif()

add_library(CLI11 INTERFACE)
target_include_directories(CLI11 INTERFACE third_party)

add_library(core_lib STATIC
        lib/ThreadPool.cpp
        lib/App.cpp
        lib/FileProcessor.cpp
        lib/utils.cpp
)

target_include_directories(core_lib PUBLIC include)

add_executable(ThreadPool src/main.cpp)

target_link_libraries(ThreadPool PRIVATE core_lib CLI11)

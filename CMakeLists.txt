cmake_minimum_required(VERSION 3.10)
project(riscv-emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")

# Source files
set(SOURCES
    src/core/cpu.cpp
    src/core/decoder.cpp
    src/core/interpreter.cpp
    src/core/elf_loader.cpp  # ADD THIS
    src/core/profiler.cpp
)

# Create library
add_library(riscv_core ${SOURCES})
target_include_directories(riscv_core PUBLIC src/core)

# Test executable (we'll add this next)
add_executable(test_cpu tests/test_cpu.cpp)
target_link_libraries(test_cpu riscv_core)
add_executable(test_interpreter tests/test_interpreter.cpp)
target_link_libraries(test_interpreter riscv_core)
add_executable(test_elf tests/test_elf.cpp)
target_link_libraries(test_elf riscv_core)
add_executable(test_profile tests/test_profile.cpp)
target_link_libraries(test_profile riscv_core)
add_executable(test_debug tests/test_debug.cpp)
target_link_libraries(test_debug riscv_core)
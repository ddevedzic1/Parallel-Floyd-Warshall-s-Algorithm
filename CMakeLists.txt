cmake_minimum_required(VERSION 3.16)
project(PAPSP LANGUAGES CXX)

# =============================================================================
# C++ Standard
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Build type (default: Release)
# =============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# =============================================================================
# Compiler Flags
# =============================================================================
if(MSVC)
    # Microsoft Visual C++
    add_compile_options(/O2 /arch:AVX2 /openmp)
else()
    # GCC / Clang
    add_compile_options(-O2 -mavx2 -fopenmp -Wall -Wextra)
    link_libraries(-fopenmp)
endif()

# =============================================================================
# OpenMP
# =============================================================================
find_package(OpenMP REQUIRED)

# =============================================================================
# Include Directories
# =============================================================================
include_directories(${CMAKE_SOURCE_DIR}/include)

# =============================================================================
# Source Files - Algorithm Library
# =============================================================================
set(ALGORITHM_SOURCES
    src/utils.cpp
    src/graph_generator.cpp
    src/io_handler.cpp
    src/algorithm_runner.cpp
    src/fw_baseline.cpp
    src/fw_baseline_unroll.cpp
    src/fw_baseline_simd.cpp
    src/fw_baseline_parallel.cpp
    src/fw_baseline_parallel_simd.cpp
    src/fw_blocking_baseline.cpp
    src/fw_blocking_unroll.cpp
    src/fw_blocking_simd.cpp
    src/fw_blocking_parallel.cpp
    src/fw_blocking_parallel_simd.cpp
    src/fw_blocking_generalized_baseline.cpp
    src/fw_blocking_generalized_unroll.cpp
    src/fw_blocking_generalized_simd.cpp
    src/fw_blocking_generalized_parallel.cpp
    src/fw_blocking_generalized_parallel_simd.cpp
)

# =============================================================================
# Static Library - All Algorithms
# =============================================================================
add_library(fw_algorithms STATIC ${ALGORITHM_SOURCES})
target_link_libraries(fw_algorithms PUBLIC OpenMP::OpenMP_CXX)

# =============================================================================
# Executable: Tests Runner
# =============================================================================
add_executable(tests_runner
    src/tests.cpp
    src/tests_runner.cpp
)
target_link_libraries(tests_runner PRIVATE fw_algorithms)

# =============================================================================
# Executable: Quick Runner
# =============================================================================
add_executable(quick_runner
    src/quick_runner.cpp
)
target_link_libraries(quick_runner PRIVATE fw_algorithms)

# =============================================================================
# Executable: Benchmark Runner
# =============================================================================
add_executable(bench_runner
    src/bench_runner.cpp
)
target_link_libraries(bench_runner PRIVATE fw_algorithms)

# =============================================================================
# Executable: Graph Generator CLI
# =============================================================================
add_executable(graph_generator_cli
    src/graph_generator_cli.cpp
)
target_link_libraries(graph_generator_cli PRIVATE fw_algorithms)

# =============================================================================
# Output Directories
# =============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# =============================================================================
# Install (Optional)
# =============================================================================
install(TARGETS tests_runner quick_runner bench_runner graph_generator_cli
        RUNTIME DESTINATION bin)

# =============================================================================
# Summary
# =============================================================================
message(STATUS "")
message(STATUS "=== PAPSP Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "OpenMP found: ${OpenMP_CXX_FOUND}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "=================================")
message(STATUS "")
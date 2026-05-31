cmake_minimum_required(VERSION 3.20)

project(reconcile_engine
    VERSION 0.1.0
    DESCRIPTION "Deterministic inventory reconciliation engine"
    LANGUAGES CXX)

# --- Language standard (pinned; fail loudly on too-old compilers) ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put all binaries in a predictable place.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Default to a Debug build when none is specified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Centralized strict-warning flags.
include(${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake)

# --- The reconciliation library ---
add_library(reconcile
    src/reconcile/schema.cpp
    src/reconcile/checksum.cpp
    src/reconcile/audit_log.cpp
    src/reconcile/reconciler.cpp)

target_include_directories(reconcile PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(reconcile PUBLIC cxx_std_20)
set_project_warnings(reconcile)

# --- The CLI binary ---
add_executable(reconcile-cli src/cli/main.cpp)
target_link_libraries(reconcile-cli PRIVATE reconcile)
set_project_warnings(reconcile-cli)

# --- Tests (opt-in; on by default, off inside FetchContent consumers) ---
option(RECONCILE_BUILD_TESTS "Build the test suite" ON)
if(RECONCILE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

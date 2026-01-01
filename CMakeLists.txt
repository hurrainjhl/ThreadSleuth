cmake_minimum_required(VERSION 3.10)
project(ParallelForensicAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -Wall")

# Include directories
include_directories(include)

# Find OpenSSL (Required for HashCalculator)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Gather source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create Main Executable
add_executable(parallel_engine ${SOURCES})
target_link_libraries(parallel_engine OpenSSL::Crypto pthread)

# Create Test Executable (Exclude main.cpp)
list(FILTER SOURCES EXCLUDE REGEX "src/main.cpp")
add_executable(test_pool tests/test_threadpool.cpp ${SOURCES})
target_link_libraries(test_pool OpenSSL::Crypto pthread)

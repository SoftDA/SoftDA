# CMake setup
cmake_minimum_required (VERSION 3.6)
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})

# Project name
project(SDA)

# Turn on the verbose
set(CMAKE_VERBOSE_MAKEFILE ON) 

# CXX target properties
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# The version number
set(SDA_VERSION_MAJOR "0")
set(SDA_VERSION_MINOR "1")
set(SDA_VERSION_PATCH "0")
set(SDA_VERSION "${SDA_VERSION_MAJOR}.${SDA_VERSION_MINOR}.${SDA_VERSION_PATCH} (alpha)")
set(SDA_HOME ${PROJECT_SOURCE_DIR})
set(SDA_UNITTEST_DIR ${PROJECT_SOURCE_DIR}/unittest)
set(SDA_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
message(STATUS "SDA_VERSION: ${SDA_VERSION}")
message(STATUS "SDA_HOME: ${SDA_HOME}")
message(STATUS "SDA_UNITTEST_DIR: ${SDA_UNITTEST_DIR}")
message(STATUS "SDA_LICENSE: ${SDA_LICENSE}")

# add the binayr tree to the search path for include files 
include_directories(${PROJECT_SOURCE_DIR})


# Add libraries to be built
add_library(SDA
  sda/headerdef.hpp 
  sda/utility/os.hpp sda/utility/os.cpp
  sda/utility/lambda.hpp
  sda/utility/singleton.hpp
  sda/utility/utility.hpp
  sda/utility/logger.hpp
  sda/utility/index.hpp
  sda/utility/tokenizer.hpp sda/utility/tokenizer.cpp
  sda/utility/scope_guard.hpp
  sda/utility/CLI11.hpp
  sda/static/logger.hpp
)


###################################################################################################
# Executables
###################################################################################################
message(STATUS "Building executables ...")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Set up linker flags
list(APPEND SDA_EXE_LINKER_FLAGS stdc++fs)
message(STATUS "SDA_EXE_LINKER_FLAGS: ${SDA_EXE_LINKER_FLAGS}")

# main binary
set(SDA_BIN ${PROJECT_SOURCE_DIR}/bin/sda)
add_executable(sda main/sda.cpp)
target_link_libraries(sda ${SDA_EXE_LINKER_FLAGS})
message(STATUS "SDA executable: ${SDA_BIN}")


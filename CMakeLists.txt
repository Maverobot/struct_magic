cmake_minimum_required(VERSION 3.11)
project(struct_magic
  VERSION 0.0.1
  LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing for this project" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Download magic_get
include(FetchContent)
FetchContent_Declare(
  magic_get
  GIT_REPOSITORY https://github.com/apolukhin/magic_get.git
  GIT_TAG 1.0.0)
FetchContent_GetProperties(magic_get)
if(NOT magic_get_POPULATED)
  FetchContent_Populate(magic_get)
endif()

add_library(magic_get INTERFACE)
target_include_directories(magic_get
  INTERFACE
  $<BUILD_INTERFACE:${magic_get_SOURCE_DIR}/include>
  )

add_library(struct_magic INTERFACE)
target_include_directories(
  struct_magic
  INTERFACE $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(struct_magic INTERFACE magic_get)
target_compile_features(struct_magic INTERFACE cxx_std_17)

# Test
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.11)
project(struct_magic)

# Download magic_get
include(FetchContent)
FetchContent_Declare(
  magic_get
  GIT_REPOSITORY https://github.com/apolukhin/magic_get.git
  GIT_TAG master)
FetchContent_GetProperties(magic_get)
if(NOT magic_get_POPULATED)
  FetchContent_Populate(magic_get)
endif()

add_executable(struct_magic_example struct_magic_example.cpp)
target_include_directories(
  struct_magic_example PRIVATE include ${magic_get_SOURCE_DIR}/include)
target_compile_features(struct_magic_example PRIVATE cxx_std_14)

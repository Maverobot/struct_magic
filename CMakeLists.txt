cmake_minimum_required(VERSION 3.11)
project(
  struct_magic
  VERSION 0.1.1
  LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing for this project" OFF)

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
target_include_directories(
  magic_get INTERFACE $<BUILD_INTERFACE:${magic_get_SOURCE_DIR}/include>
                      $<INSTALL_INTERFACE:include>)

add_library(struct_magic INTERFACE)
target_include_directories(
  struct_magic INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
target_link_libraries(struct_magic INTERFACE magic_get)
target_compile_features(struct_magic INTERFACE cxx_std_17)

# Test
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# CMake package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/struct_magicConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion)

install(
  TARGETS magic_get struct_magic
  EXPORT struct_magicTargets
  LIBRARY DESTINATION lib COMPONENT Runtime
  ARCHIVE DESTINATION lib COMPONENT Development
  RUNTIME DESTINATION bin COMPONENT Runtime
  PUBLIC_HEADER DESTINATION include COMPONENT Development
  BUNDLE DESTINATION bin COMPONENT Runtime)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/struct_magicConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/struct_magicConfig.cmake" INSTALL_DESTINATION
  lib/cmake/struct_magic)

# Install
install(EXPORT struct_magicTargets DESTINATION lib/cmake/struct_magic)
install(FILES "${PROJECT_BINARY_DIR}/struct_magicConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/struct_magicConfig.cmake"
        DESTINATION lib/cmake/struct_magic)
install(FILES ${PROJECT_SOURCE_DIR}/include/struct_magic.h DESTINATION include)
install(DIRECTORY ${magic_get_SOURCE_DIR}/include/boost DESTINATION include)

# CPack
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_CONTACT "quzhengrobot@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${struct_magic_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${struct_magic_VERSION_MINOR}")

# Debian
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
find_program(DPKG_PROG dpkg DOC "'dpkg' executable")
if(DPKG_PROG)
  execute_process(
    COMMAND ${DPKG_PROG} --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Change system name to use the correct architecture in file name
  set(CPACK_SYSTEM_NAME ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
endif()
include(CPack)

# User package registry
export(EXPORT struct_magicTargets FILE struct_magicConfig.cmake)
export(PACKAGE struct_magic)

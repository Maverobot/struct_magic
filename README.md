[![Build Status][travis-badge]][travis-link]
[![codecov][codecov-badge]][codecov-link]
[![Codacy Badge][codacy-badge]][codacy-link]
[![MIT License][license-badge]](LICENSE)
# struct_magic

Simple [header-only][header-link] libary for magical operations on struct. It is based on [magic\_get][magic-get-link], which does most of the heavy lifting.

**Table of Contents**

- [struct_magic](#structmagic)
  - [Why](#why)
  - [Usage](#usage)
    - [Examples](#examples)
    - [Cmake](#cmake)
  - [License](#license)

## Why
This package provides two functions to facilicate operation on structs:

- `struct_magic::transform`: creates a new struct whose fields are computed using a given tranform function and corresponding fields in all given structs.
- `struct_magic::apply`: apply a function on each field of a given struct.

## Usage

### Examples

Suppose we define a `struct` `Data` with

```c++
struct Data {
  double speed;
  float velocity;
  double acceleration;
  int some_integer;

  bool operator==(const Data& other) const {
    return speed == other.speed && velocity == other.velocity &&
           acceleration == other.acceleration && some_integer == other.some_integer;
  }
};
```

Following instances are defined for demonstration purpose.

```c++
Data v1{100., 11., 21., 3};
Data v2{200., 12., 22., 5};
```

- Transform each field of one struct with a lambda function
  ```c++
  auto v1_double = struct_magic::transform([](const auto& v) { return v * 2; }, v1);
  assert(v1_double == Data{200, 22, 42, 6});
  ```

- Use corresponding fields from two structs to create a new struct with a lambda function
  ```c++
  auto v3 = struct_magic::transform([](const auto& a, const auto& b) { return a + b; }, v1, v2);
  assert(v3 == Data{300, 23, 43, 8});
  ```

- Transform the struct with a lambda function
  ```c++
  struct_magic::apply(
      [i = 0](auto& field) mutable {
        field = i;
        i++;
      },
      v1);
  assert(v1 == Data{0, 1, 2, 3});
  ```

### Cmake

Add the following lines in `CMakeLists.txt` which handles the dependency on `magic_get` automatically:

```cmake
set(struct_magic_VERSION 0.1)
# Find struct_magic locally. After struct_magic is built, it will be added to user package registry which allows it to be found by find_package.
find_package(struct_magic ${struct_magic_VERSION} EXACT QUIET CONFIG)
# If struct_magic cannot be found locally, download it via fetchcontent and call add_subdirectory
if(NOT struct_magic_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    struct_magic
    GIT_REPOSITORY https://github.com/Maverobot/struct_magic.git
    GIT_TAG v${struct_magic_VERSION})
  FetchContent_GetProperties(struct_magic)
  if(NOT struct_magic_POPULATED)
    message(STATUS "Fetching struct_magic...")
    FetchContent_Populate(struct_magic)
    add_subdirectory(${struct_magic_SOURCE_DIR} ${struct_magic_BINARY_DIR})
    message(STATUS "struct_magic has been fetched.")
  endif()
endif()
```

After that, link your target against `struct_magic`:

```cmake
# Replace PRIVATE with PUBLIC if applicable
target_link_libraries(your_target PRIVATE struct_magic)
```

## License
This project is licensed under the [MIT][license].

[license]: https://github.com/Maverobot/struct_magic/blob/master/LICENSE

[travis-badge]:    https://travis-ci.com/Maverobot/struct_magic.svg?branch=master
[travis-link]:     https://travis-ci.com/Maverobot/struct_magic
[codecov-badge]:   https://codecov.io/gh/Maverobot/struct_magic/branch/master/graph/badge.svg
[codecov-link]:    https://codecov.io/gh/Maverobot/struct_magic
[codacy-badge]:    https://api.codacy.com/project/badge/Grade/b3b48e551a824caf8871a734e855bee8
[codacy-link]:     https://www.codacy.com/manual/quzhengrobot/struct_magic?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Maverobot/struct_magic&amp;utm_campaign=Badge_Grade
[license-badge]:   https://img.shields.io/badge/License-MIT-blue.svg
[magic-get-link]:  https://github.com/apolukhin/magic_get
[header-link]:     https://raw.githubusercontent.com/Maverobot/struct_magic/master/include/struct_magic.h

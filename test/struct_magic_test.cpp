/**
 * MIT License
 *
 * Copyright (c) 2020 Zheng Qu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <catch2/catch.hpp>

#include <struct_magic.h>

#include <iostream>
#include <iterator>

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

TEST_CASE("detail_util_functions", "[detail]") {
  REQUIRE_FALSE(struct_magic::detail::all_same<int, double, double>::value);
  REQUIRE(struct_magic::detail::all_same<int, int, int>::value);
}

TEST_CASE("precise-version tranform", "[struct_magic]") {
  // Structs
  Data v1{100., 11., 21., 3};
  Data v2{200., 12., 22., 5};

  // Transform each field of one struct with a lambda function
  auto v1_double = struct_magic::transform([](const auto& v) { return v * 2; }, v1);
  REQUIRE(v1_double == Data{200, 22, 42, 6});

  // Use corresponding fields from two structs to create a new struct with a lambda function
  auto v3 = struct_magic::transform([](const auto& a, const auto& b) { return a + b; }, v1, v2);
  REQUIRE(v3 == Data{300, 23, 43, 8});

  // Transform the struct with a lambda function
  struct_magic::apply(
      [i = 0](auto& item) mutable {
        item = i;
        i++;
      },
      v1);
  REQUIRE(v1 == Data{0, 1, 2, 3});
}

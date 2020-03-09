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

template <typename T, size_t n>
std::ostream& operator<<(std::ostream& o, const std::array<T, n>& arr) {
  std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
  return o;
}

TEST_CASE("detail_util_functions", "[detail]") {
  REQUIRE_FALSE(struct_magic::detail::all_same<int, double, double>::value);
  REQUIRE(struct_magic::detail::all_same<int, int, int>::value);
}

TEST_CASE("tranform", "[struct_magic]") {
  using namespace boost::pfr::ops;
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
  std::cout << "v1 before: " << v1 << std::endl;
  struct_magic::apply(
      [i = 0](auto& item) mutable {
        item = i;
        i++;
      },
      v1);
  REQUIRE(v1 == Data{0, 1, 2, 3});
}

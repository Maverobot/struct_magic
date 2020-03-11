/**
 * https://github.com/Maverobot/struct_magic
 *
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

#pragma once

#include <array>    // std::array
#include <utility>  // std::index_sequence, std::make_index_sequence

// magic_get
#include <boost/pfr/flat.hpp>
#include <boost/pfr/precise.hpp>

namespace struct_magic {

constexpr bool precise_mode = true;
constexpr bool flat_mode = false;

namespace detail {

template <size_t idx, typename... Ts>
using get_nth_type = std::tuple_element_t<idx, std::tuple<Ts...>>;

template <typename... Conds>
struct all_true : std::true_type {};
template <typename Cond, typename... Conds>
struct all_true<Cond, Conds...>
    : std::conditional<Cond::value, all_true<Conds...>, std::false_type>::type {};

template <typename... Ts>
struct all_same : all_true<std::is_same<get_nth_type<0, Ts...>, Ts>...> {};

template <bool mode, size_t index, typename Func, typename... Ts>
auto transform(Func&& f, Ts&&... input) {
  if constexpr (mode == precise_mode) {
    return std::forward<Func>(f)(boost::pfr::get<index>(std::forward<Ts>(input))...);
  } else {
    return std::forward<Func>(f)(boost::pfr::flat_get<index>(std::forward<Ts>(input))...);
  }
}

template <bool mode, typename Func, size_t... indices, typename... Ts>
auto transform(Func&& f, std::index_sequence<indices...>, Ts&&... input) {
  return std::decay_t<get_nth_type<0, Ts...>>{
      transform<mode, indices>(std::forward<Func>(f), std::forward<Ts>(input)...)...};
}

template <bool mode, typename Op, typename Struct, typename std::size_t... Is>
void apply_dispatch(Op&& op, Struct&& s, std::index_sequence<Is...>) {
  if constexpr (mode == precise_mode) {
    ((void)std::forward<Op>(op)(boost::pfr::get<Is>(std::forward<Struct>(s))), ...);
  } else {
    ((void)std::forward<Op>(op)(boost::pfr::flat_get<Is>(std::forward<Struct>(s))), ...);
  }
}  // namespace detail

}  // namespace detail

// Apply a transformation to turn a "list" of structs into one struct
template <bool mode = precise_mode,
          typename Func,
          typename... Ts,
          typename = std::enable_if_t<detail::all_same<Ts...>::value, int>>
auto transform(Func&& f, Ts&&... input) {
  if constexpr (mode == precise_mode) {
    return detail::transform<precise_mode>(
        std::forward<Func>(f),
        std::make_index_sequence<
            boost::pfr::tuple_size<std::decay_t<detail::get_nth_type<0, Ts...>>>::value>(),
        std::forward<Ts>(input)...);
  } else {
    return detail::transform<flat_mode>(
        std::forward<Func>(f),
        std::make_index_sequence<
            boost::pfr::flat_tuple_size<std::decay_t<detail::get_nth_type<0, Ts...>>>::value>(),
        std::forward<Ts>(input)...);
  }
}

// Apply a transformation function on all elements of a struct and return a tuple of results
template <bool mode = precise_mode, typename Op, typename Struct>
void apply(Op&& op, Struct&& s) {
  if constexpr (mode == precise_mode) {
    detail::apply_dispatch<precise_mode>(
        op, std::forward<Struct>(s),
        std::make_index_sequence<boost::pfr::tuple_size<std::decay_t<Struct>>::value>{});
  } else {
    detail::apply_dispatch<flat_mode>(
        op, std::forward<Struct>(s),
        std::make_index_sequence<boost::pfr::flat_tuple_size<std::decay_t<Struct>>::value>{});
  }
}

}  // namespace struct_magic

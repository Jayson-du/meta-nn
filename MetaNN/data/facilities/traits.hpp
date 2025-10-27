#pragma once

// #include "device_tag.hpp"

#include <iterator>
#include <type_traits>

namespace MetaNN {
template <typename T> struct IsIterator_ {

  template <typename R>
  static std::true_type
  Test(typename std::iterator_traits<R>::iterator_category *);

  template <typename R> static std::false_type Test(...);

  static constexpr bool value = decltype(Test<T>(nullptr))::value;
};

template <typename T> constexpr bool IsIterator = IsIterator_<T>::value;

// template <typename T1, typename T2,
// std::enable_if_t<IsValidCategoryTag<type
} // namespace MetaNN
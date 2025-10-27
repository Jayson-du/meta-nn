#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace MetaNN {
  template <typename T>
  struct Identity_ {
    using type = T;
  }
}
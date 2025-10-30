#pragma once

#include "helpers.hpp"
#include <tuple>

namespace MetaNN::Map {

namespace NSCrateFromItems {
template <template <typename> typename KeyPicker> struct KVCreator {
  template <typename TItem> struct apply {
    // 因为编译器无法确定`KeyPicker<TItem>::type`是否为类型，所以需要加上
    // `typename` 关键字 否则会报错 `expected a type, got
    // 'KeyPicker<TItem>::type'`
    // 编译器默认将`KeyPicker<TItem>::type`解析为静态成员变量
    using type = Helper::KVBinder<typename KeyPicker<TItem>::type, TItem>;
  };
};
} // namespace NSCrateFromItems

} // namespace MetaNN::Map
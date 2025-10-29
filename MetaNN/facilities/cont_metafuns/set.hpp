#pragma once

#include "sequential.hpp"

namespace MetaNN::Set {

// @brief : 实现编译期具名词典
namespace NSHasKey {

template <typename TCont> struct map_;

// struct map_<TCon<TItem...>> : Helper::KVBinder<TItem, Helper::Int_<true>>...
// 是包展开，表示对每个 TItem 都生成一个基类 `KVBinder<TItem,
// Helper::Int_<true>>`
template <template <typename...> typename TCon, typename... TItem>
struct map_<TCon<TItem...>> : Helper::KVBinder<TItem, Helper::Int_<true>>... {
  // 包展开的 using 声明，把每个基类的 apply 都声明出来
  using Helper::KVBinder<TItem, Helper::Int_<true>>::apply...;
  static Helper::Int_<false> apply(...);
};
} // namespace NSHasKey

template <typename TCon, typename TKey> struct HasKey_ {
  constexpr static bool value =
      decltype(NSHasKey::map_<TCon>::apply((TKey *)nullptr))::value;
};

template <typename TCon, typename TKey>
constexpr bool HasKey = HasKey_<TCon, TKey>::value;

// brief : 构建一个自动插入类型的模板
template <typename TCon, typename TKey, bool bMute = false>
struct Insert_ : Sequential::PushBack_<TCon, TKey> {
  static_assert(!HasKey<TCon, TKey>);
};

template <typename TCon, typename TKey> struct Insert_<TCon, TKey, true> {
  using type =
      typename std::conditional<HasKey<TCon, TKey>, Identity_<TCon>,
                                Sequential::PushBack_<TCon, TKey>>::type;
};

template <typename TCon, typename TKey, bool bMute = false>
using Insert = typename Insert_<TCon, TKey, bMute>::type;

// @brief : 构建一个类型擦除模板
namespace NSErase {
template <typename TCon, typename TKey, typename... TItems> struct Helper_ {
  using type = TCon;
};

// @brief Helper_特化, 擦除 `TKey` 后的类型
// @tparam TCon 容器
// @tparam TParams 可变模板参数
// @tparam TKey
// @tparam TCur
// @tparam TItems
template <template <typename...> typename TCon, typename... TParams,
          typename TKey, typename TCur, typename... TItems>
struct Helper_<TCon<TParams...>, TKey, TCur, TItems...> {
  using type = typename Helper_<TCon<TParams..., TCur>, TKey, TItems...>::type;
};

template <template <typename...> typename TCon, typename... TParams,
          typename TKey, typename... TItems>
struct Helper_<TCon<TParams...>, TKey, TKey, TItems...> {
  using type = TCon<TParams..., TItems...>;
};
} // namespace NSErase

template <typename TCon, typename TKey> struct Erase_;

template <template <typename...> typename TCon, typename TKey,
          typename... TItems>
struct Erase_<TCon<TItems...>, TKey>
    : NSErase::Helper_<TCon<>, TKey, TItems...> {};

template <typename TCon, typename TKey>
using Erase = typename Erase_<TCon, TKey>::type;

namespace NSCreateFromItems {

template <template <typename> typename Picker, bool bMute> struct Creator {
  template <typename TState, typename TInput> struct apply {
    using TItem = typename Picker<TInput>::type;
    using type = Insert<TState, TItem, bMute>;
  };
};
} // namespace NSCreateFromItems

template <typename TItemCont, template <typename> typename Picker, bool bMute,
          template <typename...> typename TOutCont = std::tuple>
struct CreateFromItems_ {
  using type = Sequential::Fold<
      TOutCont<>, TItemCont,
      NSCreateFromItems::Creator<Picker, bMute>::template apply>;
};

template <typename TItemCont, template <typename> typename Picker, bool bMute,
          template <typename...> typename TOutCont = std::tuple>
using CreateFromItems =
    typename CreateFromItems_<TItemCont, Picker, bMute, TOutCont>::type;

// Equals ======================================================================
template <typename TFirstSet, typename TSecondSet> struct IsEqual_;

template <template <typename...> class Cont1,
          template <typename...> class Cont2, typename... Params1,
          typename... Params2>
struct IsEqual_<Cont1<Params1...>, Cont2<Params2...>> {
  constexpr static bool value1 = (HasKey<Cont1<Params1...>, Params2> && ...);
  constexpr static bool value2 = (HasKey<Cont2<Params2...>, Params1> && ...);
  constexpr static bool value = value1 && value2;
};

template <typename TFirstSet, typename TSecondSet>
constexpr bool IsEqual = IsEqual_<TFirstSet, TSecondSet>::value;
} // namespace MetaNN::Set
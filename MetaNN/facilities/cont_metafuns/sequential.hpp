#pragma once

#include "helpers.hpp"

#include <cstddef>

namespace MetaNN::Sequencetial {
namespace NSCreate {
template <typename L, typename R> struct Concat;

template <template <typename...> typename TCont, typename... L, typename... R>
struct Concat<TCont<L...>, TCont<R...>> {
  using type = TCont<L..., R...>;
};
} // namespace NSCreate

// @brief
// @tparam TCont
// @tparam TParam
// @tparam N
template <template <typename...> typename TCont, typename TParam, int N>
struct Create_ {
  using type = typename NSCreate::Concat<
      typename Create_<TCont, TParam, N / 2>::type,
      typename Create_<TCont, TParam, N - N / 2>::type>::type;
};

template <template <typename...> typename TCont, typename TParam>
struct Create_<TCont, TParam, 1> {
  using type = TCont<TParam>;
};

template <template <typename...> typename TCont, typename TParam>
struct Create_<TCont, TParam, 0> {
  using type = TCont<>;
};

template <template <typename...> typename TCont, typename TParam, int N>
using Create = typename Create_<TCont, TParam, N>::type;

// @brief : 通过逗号表达式和decltype(args), 生成一个编译时确定的类型索引
// 主要原理是:
// template <typename nth>
// static nth apply(decltype(ignore, (void *)nullptr)..., nth *, ...);
// apply 返回 'nth' 类型,
// 那么参数包中其他类型都被 'decltype(ignore, (void *)nullptr)...'设置为'void
// *'类型
// At_使用Helper::MakeIndexSequence<N>生成一个编译期整数序列<0, 1, ..., N - 1>,
// 用于辅助参数包展开
// 使用NSAt::impl<...>::apply((TParams*)nullptr...)
// 通过参数包展开，把每个类型都转成指针
// (TParams*)nullptr，然后使用重载，只有第 N 个类型会匹配到 nth*
// 参数，其余都被丢弃。
// decltype(...)
// 得到 apply 返回值的类型（其实就是第 N
// 个类型的指针类型），再通过模板推导把它变成类型本身。
// using type = decltype(...)
// 最终，type 就是 TParams... 里的第 N 个类型
namespace NSAT {
template <typename ignore> struct impl;

template <int... ignore> struct impl<Helper::IndexSequence<ignore...>> {

  template <typename nth>
  static nth apply(decltype(ignore, (void *)nullptr)..., nth *, ...);
};
} // namespace NSAT

template <typename TCon, int N> struct At_;

template <template <typename...> typename TCon, typename... TParams, int N>
struct At_<TCon<TParams...>, N> {

  using type = decltype(NSAT::impl<Helper::MakeIndexSequence<N>>::apply(
      (TParams *)nullptr...));
};

template <typename TCon, int N> using At = typename At_<TCon, N>::type;

namespace NSOrder {
// @brief : 模板参数包展开 + 多重继承 + using声明包展开
// Helper::KVBinder<TTypes, Helper::Int_<index>>... 表示包展开多重继承
// 让 `impl` 继承自每一个 `KVBinder<TTypes, Int_<index>>`,

template <typename TIndexCont, typename TTypeCont> struct impl;

template <template <typename...> typename TTypeCont, typename... TTypes,
          int... index>
struct impl<Helper::IndexSequence<index...>, TTypeCont<TTypes...>>
    : Helper::KVBinder<TTypes, Helper::Int_<index>>... {
  // using声明的包展开，把每个基类里的 apply
  // 成员都引入到当前作用域，实现“查找表”功能
  using Helper::KVBinder<TTypes, Helper::Int_<index>>::apply...;
};
} // namespace NSOrder

template <typename TCon, typename TReq> struct Order_;

template <template <typename...> typename TCon, typename... TParams,
          typename TReq>
struct Order_<TCon<TParams...>, TReq> {
  using IndexSeq = Helper::MakeIndexSequence<sizeof...(TParams)>;
  using LookUpTable = NSOrder::impl<IndexSeq, TCon<TParams...>>;
  using AimType = decltype(LookUpTable::apply((TReq *)nullptr));
  constexpr static int value = AimType::value;
};

template <typename TCon, typename TReq>
constexpr static int Order = Order_<TCon, TReq>::value;

} // namespace MetaNN::Sequencetial
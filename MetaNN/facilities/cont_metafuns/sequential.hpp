#pragma once

#include "helpers.hpp"

#include <cstddef>

namespace MetaNN::Sequential {
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

  using type = decltype(
      NSAT::impl<Helper::MakeIndexSequence<N>>::apply((TParams *)nullptr...));
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

// @brief : 在类型序列的第 'N' 个位置, 将原始的 'TN' 替换为一个类型 'TValue'
namespace NSSet {

// brief : 基础模板, 为后续特化做准备
template <typename TCon, int N, typename TValue, typename TRemain,
          typename = Helper::When<true>>
struct imp_;

// brief : 特化 N == 0 的情况
template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue, TCon<T0, TRemain...>,
            Helper::When<(N == 0)>> {
  using type = TCon<TProcessed..., TValue, TRemain...>;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue, TCon<T0, T1, TRemain...>,
            Helper::When<(N == 1)>> {
  using type = TCon<TProcessed..., T0, TValue, TRemain...>;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename T2,
          typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue, TCon<T0, T1, T2, TRemain...>,
            Helper::When<(N == 2)>> {
  using type = TCon<TProcessed..., T0, T1, TValue, TRemain...>;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename T2,
          typename T3, typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue, TCon<T0, T1, T2, T3, TRemain...>,
            Helper::When<(N == 3)>> {
  using type = TCon<TProcessed..., T0, T1, T2, TValue, TRemain...>;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename T2,
          typename T3, typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue, TCon<T0, T1, T2, T3, TRemain...>,
            Helper::When<(N >= 4) && (N < 8)>> {
  using type = typename imp_<TCon<TProcessed..., T0, T1, T2, T3>, N - 4, TValue,
                             TCon<TRemain...>>::type;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename T2,
          typename T3, typename T4, typename T5, typename T6, typename T7,
          typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue,
            TCon<T0, T1, T2, T3, T4, T5, T6, T7, TRemain...>,
            Helper::When<(N >= 8) && (N < 16)>> {
  using type =
      typename imp_<TCon<TProcessed..., T0, T1, T2, T3, T4, T5, T6, T7>, N - 8,
                    TValue, TCon<TRemain...>>::type;
};

template <template <typename...> typename TCon, int N, typename TValue,
          typename... TProcessed, typename T0, typename T1, typename T2,
          typename T3, typename T4, typename T5, typename T6, typename T7,
          typename T8, typename T9, typename TA, typename TB, typename TC,
          typename TD, typename TE, typename TF, typename... TRemain>
struct imp_<TCon<TProcessed...>, N, TValue,
            TCon<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD, TE, TF,
                 TRemain...>,
            Helper::When<(N >= 16)>> {
  using type = typename imp_<TCon<TProcessed..., T0, T1, T2, T3, T4, T5, T6, T7,
                                  T8, T9, TA, TB, TC, TD, TE, TF>,
                             N - 16, TValue, TCon<TRemain...>>::type;
};
} // namespace NSSet

template <typename TCont, int N, typename TValue> struct Set_;

template <template <typename...> typename TCont, int N, typename TValue,
          typename... TParams>
struct Set_<TCont<TParams...>, N, TValue> {
  using type =
      typename NSSet::imp_<TCont<>, N, TValue, TCont<TParams...>>::type;
};

template <typename TCont, int N, typename TValue>
using Set = typename Set_<TCont, N, TValue>::type;

// brief : Fold
namespace NSFold {
template <typename TState, template <typename, typename> typename F,
          typename... TRemain>
struct imp_ {
  using type = TState;
};

template <typename TState, template <typename, typename> typename F,
          typename T0>
struct imp_<TState, F, T0> {
  using type = F<TState, T0>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1>
struct imp_<TState, F, T0, T1> {
  using type = F<F<TState, T0>, T1>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1, typename T2>
struct imp_<TState, F, T0, T1, T2> {
  using type = F<F<F<TState, T0>, T1>, T2>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1, typename T2, typename T3>
struct imp_<TState, F, T0, T1, T2, T3> {
  using type = F<F<F<F<TState, T0>, T1>, T2>, T3>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1, typename T2, typename T3, typename T4>
struct imp_<TState, F, T0, T1, T2, T3, T4> {
  using type = F<F<F<F<F<TState, T0>, T1>, T2>, T3>, T4>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1, typename T2, typename T3, typename T4,
          typename T5>
struct imp_<TState, F, T0, T1, T2, T3, T4, T5> {
  using type = F<F<F<F<F<F<TState, T0>, T1>, T2>, T3>, T4>, T5>;
};

template <typename TState, template <typename, typename> typename F,
          typename T0, typename T1, typename T2, typename T3, typename T4,
          typename T5, typename T6, typename... TRemain>
struct imp_<TState, F, T0, T1, T2, T3, T4, T5, T6, TRemain...> {
  using type =
      typename imp_<F<F<F<F<F<F<F<TState, T0>, T1>, T2>, T3>, T4>, T5>, T6>, F,
                    TRemain...>::type;
};
} // namespace NSFold

template <typename TInitState, typename TInputCont,
          template <typename, typename> typename F>
struct Fold_;

template <typename TInitState, template <typename...> typename TCont,
          typename... TParams, template <typename, typename> typename F>
struct Fold_<TInitState, TCont<TParams...>, F> {
  template <typename S, typename I> using FF = typename F<S, I>::type;

  using type = typename NSFold::imp_<TInitState, FF, TParams...>::type;
};

template <typename TInitState, typename TInputCont,
          template <typename, typename> typename F>
using Fold = typename Fold_<TInitState, TInputCont, F>::type;

// brief : PushBack, 在类型序列的末尾添加新的类型
template <typename TCont, typename... TValue> struct PushBack_;

template <template <typename...> typename TCont, typename... TParams,
          typename... TValue>
struct PushBack_<TCont<TParams...>, TValue...> {
  using type = TCont<TParams..., TValue...>;
};

template <typename TCont, typename... TValue>
using PushBack = typename PushBack_<TCont, TValue...>::type;

// brief : Cascade, 连接两个类型序列
template <typename TCont1, typename TCont2> struct Cascade_;

template <template <typename...> typename TCont, typename... TParams1,
          typename... TParams2>
struct Cascade_<TCont<TParams1...>, TCont<TParams2...>> {
  using type = TCont<TParams1..., TParams2...>;
};

// brief :
template <typename TInCont, template <typename> typename F,
          template <typename...> typename TOutCont>
struct Transform_;

template <template <typename...> typename TInCont, typename... TInputs,
          template <typename> typename F,
          template <typename...> typename TOutCont>
struct Transform_<TInCont<TInputs...>, F, TOutCont> {
  using type = TOutCont<typename F<TInputs>::type...>;
};

template <typename TInCont, template <typename> typename F,
          template <typename...> typename TOutCont>
using Transform = typename Transform_<TInCont, F, TOutCont>::type;

} // namespace MetaNN::Sequential

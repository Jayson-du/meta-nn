#pragma once

namespace MetaNN::Helper {
// @brief : 模板'int'类型
// @tparam N : 非类型模板参数
template <auto N> struct Int_ { constexpr static auto value = N; };

template <int... I> struct IndexSequence;

namespace NSIndexSequence {
template <typename L, typename R> struct concat;

template <int... L, int... R>
struct concat<IndexSequence<L...>, IndexSequence<R...>> {
  using type = IndexSequence<L..., (R + sizeof...(L))...>;
};
} // namespace NSIndexSequence

// @brief 创建一个0...N的索引队列
// @tparam N
template <int N> struct MakeIndexSequence_ {
  using type = typename NSIndexSequence::concat<
      typename MakeIndexSequence_<N / 2>::type,
      typename MakeIndexSequence_<N - N / 2>::type>::type;
};

// @brief : 只有1个索引
// @tparam
template <> struct MakeIndexSequence_<1> { using type = IndexSequence<0>; };

// @brief : 空索引
// @tparam
template <> struct MakeIndexSequence_<0> { using type = IndexSequence<>; };

template <int N> using MakeIndexSequence = typename MakeIndexSequence_<N>::type;

template <typename TK, typename TV> struct KVBinder {
  using KeyType = TK;
  using ValueType = TV;
  static TV apply(TK *);
};

template <bool b> struct When;

template <typename... TValues> struct ValueSequence;

template <typename V1, typename V2> struct Pair {
  using FirstType = V1;
  using SecondType = V2;
};

} // namespace MetaNN::Helper
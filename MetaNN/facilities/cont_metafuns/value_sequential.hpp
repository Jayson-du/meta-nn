#pragma once

#include "helpers.hpp"

// brief : 创建判断包含与顺序的元函数

namespace MetaNN::ValueSequential {
template <typename TValueSeq, auto value> struct Contains_;

template <template <auto...> typename TValueCont, auto val, auto... vals>
struct Contains_<TValueCont<vals...>, val> {
  constexpr static bool value = ((vals == val) || ...);
};

template <typename TValueSeq, auto val>
constexpr static bool Contains = Contains_<TValueSeq, val>::value;

// order
namespace NSOrder {
template <typename TIndexCont, typename TTypeCont> struct impl;

// brief : 为每个TTypes中的值，建立一个“值到索引”的查找表(类型映射)
//         TTypes中的值必须唯一存在
template <template <auto...> typename TTypeCont, auto... TTypes, int... index>
struct impl<Helper::IndexSequence<index...>, TTypeCont<TTypes...>>
    : Helper::KVBinder<Helper::Int_<TTypes>, Helper::Int_<index>>... {
  using Helper::KVBinder<Helper::Int_<TTypes>, Helper::Int_<index>>::apply...;
};
} // namespace NSOrder

template <typename TCon, auto TReq> struct Order_;

template <template <auto...> typename TCon, auto... TParams, auto TReq>
struct Order_<TCon<TParams...>, TReq> {
  using IndexSeq = Helper::MakeIndexSequence<sizeof...(TParams)>;
  // 构建查找表, LookUpTable是 "值到索引" 的查找表类型。
  using LookUpTable = NSOrder::impl<IndexSeq, TCon<TParams...>>;

  // 查找TReq对应的类型
  using ReqType = Helper::Int_<TReq>;

  // 查找TReq对应的索引
  using AimType = decltype(LookUpTable::apply((ReqType *)nullptr));

  // 获取索引值
  constexpr static int value = AimType::value;
};

template <typename TCon, auto TReq>
constexpr static int Order = Order_<TCon, TReq>::value;
} // namespace MetaNN::ValueSequential
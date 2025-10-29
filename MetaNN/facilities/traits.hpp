#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

// @brief: 类型萃取文件, 提供多种萃取器

namespace MetaNN {

// @brief : 类型萃取器
// @tparam T
template <typename T> struct Identity_ { using type = T; };

// @brief : 编译时或操作
// @tparam cur
// @tparam TNext
template <bool cur, typename TNext> constexpr static bool OrValue = true;

// @brief : 偏特化编译时或操作
// @tparam TNext
template <typename TNext>
constexpr static bool OrValue<false, TNext> = TNext::value;

// @brief : 编译时与操作
// @tparam cur
// @tparam TNext
template <bool cur, typename TNext> constexpr static bool AndValue = false;

// @brief : 偏特化编译时与操作
// @tparam TNext
template <typename TNext>
constexpr static bool AndValue<true, TNext> = TNext::value;

// @brief 去除cv限制和引用
// @tparam T
template <typename T>
using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;

// @brief 编译时依赖错误声明
// @tparam T
template <typename... T> constexpr static bool DependencyFalse = false;

// @brief 编译时条件选择器
// @tparam TBoolleanCont
// @tparam TFunCont
template <typename TBoolleanCont, typename TFunCont> struct CompileTimeSwitch_;

// @brief : 编译时条件选择器
// @tparam curBool : 如果curBool为true, 则选择curFunc
// @tparam TBools : 可变参数模板
// @tparam TFunCont
// @tparam curFunc
// @tparam TFuncs
template <bool curBool, bool... TBools, template <typename...> class TFunCont,
          typename curFunc, typename... TFuncs>
struct CompileTimeSwitch_<std::integer_sequence<bool, curBool, TBools...>,
                          TFunCont<curFunc, TFuncs...>> {
  static_assert((sizeof...(TBools) == sizeof...(TFuncs)) ||
                (sizeof...(TBools) + 1 == sizeof...(TFuncs)));

  // 如果curBool为true, 则选择curFunc,
  // 若干curBool为false, 则选择递归选择
  using type = typename std::conditional_t<
      curBool, Identity_<curFunc>,
      CompileTimeSwitch_<std::integer_sequence<bool, TBools...>,
                         TFunCont<TFuncs...>>>::type;
};

// @brief : 递归终止
// @tparam TFunCont
// @tparam curFunc
template <template <typename...> class TFunCont, typename curFunc>
struct CompileTimeSwitch_<std::integer_sequence<bool>, TFunCont<curFunc>> {
  using type = curFunc;
};

// @brief 编译选择器模板元
// @tparam TBoolleanCont
// @tparam TFunCont
template <typename TBoolleanCont, typename TFunCont>
using CompileTimeSwitch =
    typename CompileTimeSwitch_<TBoolleanCont, TFunCont>::type;
} // namespace MetaNN
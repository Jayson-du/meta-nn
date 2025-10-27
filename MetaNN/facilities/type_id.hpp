#pragma once

#include <atomic>
#include <cstddef>

namespace MetaNN {
namespace NSTypeID {

// @brief 创建类型ID号
// @tparam T
// @return size_t
inline size_t GenTypID() {
  static std::atomic<size_t> m_counter = 0;
  return m_counter.fetch_add(1);
}
} // namespace NSTypeID

// @brief 创建类型ID号
// @tparam T
// @return size_t
template <typename T> size_t TypeID() {
  const static size_t id = NSTypeID::GenTypID();
  return id;
}
} // namespace MetaNN

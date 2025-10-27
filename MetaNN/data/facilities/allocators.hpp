#pragma once

#include "device_tag.hpp"

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace MetaNN {
template <typename TDevieceTag> struct Allocator;

// CPU设备内存分配器
template <> struct Allocator<DeviceTags::CPU> {
private:
  // @brief: 自定义内存分配器
  struct AllocHelper {
    std::unordered_map<size_t, std::deque<void *>> memBuffer;
    // 释放内存
    ~AllocHelper() {
      for (auto &item : memBuffer) {
        auto &refVec = item.second;
        for (auto &p1 : refVec) {
          char *buf = (char *)(p1);
          delete[] buf;
        }
        refVec.clear();
      }
    }
  };

  // @brief: 自定义析构器
  struct DesImpl {
    DesImpl(std::deque<void *> &p_refPool) : m_refPool(p_refPool) {}

    void operator()(void *p_val) {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_refPool.push_back(p_val);
    }

  private:
    std::deque<void *> &m_refPool;
  };

public:
  template <typename T> static std::shared_ptr<T> Alloc(size_t count) {
    if (count == 0) {
      return std::shared_ptr<T>(nullptr);
    }
    count *= sizeof(T);

    if (count & 0x3ff) {
      count = ((count >> 10) + 1) << 10;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    static AllocHelper helper;
    auto &slot = helper.memBuffer[count];
    if (slot.empty()) {
      auto rawPtr = (T *)(new char[count]);
      return std::shared_ptr<T>(rawPtr, DesImpl(slot));
    } else {
      void *mem = slot.back();
      slot.pop_back();
      return std::shared_ptr<T>((T *)mem, DesImpl(slot));
    }
  }

private:
  inline static std::mutex m_mutex;
};
} // namespace MetaNN
// brief ： 测试书序字典

#include "MetaNN/facilities/cont_metafuns/set.hpp"

#include <iostream>
#include <tuple>

struct A {
  void testA() { std::cout << __func__ << std::endl; }
};

struct B {
  void testB() { std::cout << __func__ << std::endl; }
};

struct C {
  void testC() { std::cout << __func__ << std::endl; }
};

struct D {
  void testD() { std::cout << __func__ << std::endl; }
};

using meta_set = std::tuple<A, B, C>;

using meta_map = MetaNN::Set::NSHasKey::map_<meta_set>;

int main(int argc, char *argv[]) {

  static_assert(std::is_same_v<decltype(meta_map::apply((A *)nullptr)),
                               MetaNN::Helper::Int_<true>>,
                "A exist");
  static_assert(std::is_same_v<decltype(meta_map::apply((D *)nullptr)),
                               MetaNN::Helper::Int_<false>>,
                "D exist");

  if constexpr (MetaNN::Set::NSHasKey::HasKey<meta_set, A>) {
    std::cout << "A exits" << std::endl;
  }

  if constexpr (!MetaNN::Set::NSHasKey::HasKey<meta_set, D>) {
    std::cout << "D not exits" << std::endl;
  }

  return 0;
}
#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include <algorithm>

namespace fm {

template <typename T> struct resource_traits {};

template <> struct resource_traits<int> { constexpr static int initial = -1; };

template <typename T> struct resource_traits<T *> {
  constexpr static T *initial = nullptr;
};

template <typename T, typename Traits = resource_traits<T>>
class basic_resource {
protected:
  T t = Traits::initial;

public:
  basic_resource() = default;
  basic_resource(basic_resource &&other) : t(other.t) {
    other.t = Traits::initial;
  };
  ~basic_resource() {
    if (t != Traits::initial)
      Traits::destructor(t);
  };

  template <typename... Args>
  basic_resource(Args &&... args) : t(Traits::constructor(args...)) {}

  basic_resource(const basic_resource &) = delete;
  basic_resource &operator=(const basic_resource &) = delete;
  basic_resource &operator=(basic_resource &&) = delete;

  static basic_resource steal(T &t) {
    basic_resource resource;
    std::swap(resource.t, t);
    return resource;
  }

  operator T() { return t; }

  T *release() {
    T t1 = t;
    t = Traits::initial;
    return t1;
  }
};
}

#endif
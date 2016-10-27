#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include <algorithm>

namespace fm {

// Resource types are configured by resource traits that specify default values
// as defined by the C API. They also specify the C API equivalents for factory
// and cleanup functions. In general, C factories tend to return a value of
// fundamental type and the cleanup functions take that value as its sole
// argument. For C APIs that break this general pattern, this will be handled by
// the resource_trait specialization for that type.
template <typename T> struct resource_traits {};

// Resources that are descriptors tend to be integers, with -1 being a value
// that represents a null resource. C APIs that break this general pattern will
// be handled by a resource_trait specialization for that type.
template <> struct resource_traits<int> { constexpr static int initial = -1; };

// Resources that are opaque objects tend to be pointers. All pointer types
// basically has nullptr has the null resource.
template <typename T> struct resource_traits<T *> {
  constexpr static T *initial = nullptr;
};

template <typename T, typename Traits = resource_traits<T>>
class basic_resource {
protected:
  // Atomicity should follow that of other resources, like fstreams.
  T t = Traits::initial;

public:
  basic_resource() = default;
  basic_resource(basic_resource &&other) : t(other.t) {
    other.t = Traits::initial;
  }
  basic_resource &operator=(basic_resource &&other) {
    if (t != Traits::initial)
      Traits::destructor(t);
    t = other.t;
    other.t = Traits::initial;
  }
  ~basic_resource() {
    if (t != Traits::initial)
      Traits::destructor(t);
  }

  // This constructor wraps the C API factory function only.
  template <typename... Args>
  basic_resource(Args &&... args) : t(Traits::constructor(args...)) {}

  // Most resources are not copyable or copy assignable.
  basic_resource(const basic_resource &) = delete;
  basic_resource &operator=(const basic_resource &) = delete;

  // This is to make it very clear that a C resource is being managed and
  // prevents it from being used unmanaged at the same time. A constructor
  // taking an r-value is unfamiliar in a C context. Verbs like "acquire" does
  // not express the extent to which automatic resource management requires
  // complete ownership.
  static basic_resource steal(T &t) {
    basic_resource resource;
    std::swap(resource.t, t);
    return resource;
  }

  // C resources should be still usable in a C-like manner, so being passed to
  // the C API that use them should induce the underlying type in a
  // natural way.
  operator T() { return t; }

  // Like unique_ptr, frees the C resource from automatic management.
  T release() {
    auto t1 = t;
    t = Traits::initial;
    return t1;
  }
};
}

#endif
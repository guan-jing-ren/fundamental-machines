#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

namespace fm {
template <typename T = int, T initial = -1, T error = initial>
class basic_resource {
  T t = initial;

public:
};

template <typename T>
class basic_resource<T *> : public basic_resource<T *, nullptr> {
  T t = nullptr;

public:
};

#include "basic_resource.ipp"
}

#endif
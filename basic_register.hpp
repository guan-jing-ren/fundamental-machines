#ifndef _BASIC_REGISTER_H_
#define _BASIC_REGISTER_H_

#include <type_traits>

namespace fm {
template <typename E, std::size_t... fields> struct basic_register {
  static_assert(std::is_enum<E>::value,
                "Field accessors must be enumerated type");
};
}

#endif
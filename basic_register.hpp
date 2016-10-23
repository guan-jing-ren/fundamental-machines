#ifndef _BASIC_REGISTER_H_
#define _BASIC_REGISTER_H_

#include <type_traits>
#include <limits>
#include <utility>

namespace fm {
namespace detail {
template <std::size_t... fields>
constexpr auto register_size(std::integer_sequence<std::size_t, fields...>) {
  std::size_t size = 0;
  for (auto f : {fields...})
    size += f;
  return size;
}

constexpr auto power_of_two(std::size_t s) {
  if (s == 1)
    return true;
  if (s % 2 == 1)
    return false;
  return power_of_two(s / 2);
}
}

template <typename E, std::size_t... fields> struct basic_register {
  static_assert(std::is_enum<E>::value,
                "Field accessors must be enumerated type");
  static_assert(detail::power_of_two(detail::register_size(
                    std::integer_sequence<std::size_t, fields...>{})),
                "Total register size not a power of two");
  static_assert(std::numeric_limits<std::underlying_type_t<E>>::max() >=
                    sizeof...(fields),
                "Field accessor is not large enough to address all fields");
};
}

#endif
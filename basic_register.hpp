#ifndef _BASIC_REGISTER_H_
#define _BASIC_REGISTER_H_

#include <type_traits>
#include <limits>
#include <cstdint>
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

template <std::size_t N> struct unit { using type = void; };
template <> struct unit<8> { using type = std::uint8_t; };
template <> struct unit<16> { using type = std::uint16_t; };
template <> struct unit<32> { using type = std::uint32_t; };
template <> struct unit<64> { using type = std::uint64_t; };
template <std::size_t N> using unit_t = typename unit<N>::type;
}

template <typename E, std::size_t... fields> struct basic_register {
  constexpr static std::size_t size =
      detail::register_size(std::integer_sequence<std::size_t, fields...>{});
  using field_accessor_type = std::remove_pointer_t<E>;
  using value_type = detail::unit_t<size>;
  using type =
      std::conditional_t<!std::is_pointer<E>::value, volatile value_type,
                         volatile value_type *const volatile>;

  static_assert(std::is_enum<field_accessor_type>::value,
                "Field accessors must be enumerated type");
  static_assert(detail::power_of_two(size),
                "Total register size not a power of two");
  static_assert(
      std::numeric_limits<std::underlying_type_t<field_accessor_type>>::max() >=
          sizeof...(fields),
      "Field accessor is not large enough to address all fields");

  type r;

  template <typename T> basic_register(T r) : r(reinterpret_cast<type>(r)) {}
};
}

#endif
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

template <typename E, std::size_t... fields>
constexpr std::size_t sum(E e, std::integer_sequence<std::size_t, fields...>) {
  constexpr std::size_t fs[] = {fields...};
  std::size_t total = 0;
  std::size_t e1 = 0;
  for (auto f : fs) {
    if (static_cast<std::size_t>(e) == e1++)
      break;
    total += f;
  }
  return total;
}
}

template <typename E, std::size_t... fields> struct basic_register {
  constexpr static std::size_t size =
      detail::register_size(std::integer_sequence<std::size_t, fields...>{});
  using field_accessor_type = std::remove_pointer_t<E>;
  using value_type = detail::unit_t<size>;
  using type =
      std::conditional_t<!std::is_pointer<E>::value, volatile value_type,
                         volatile value_type *const>;

  static_assert(std::is_enum<field_accessor_type>::value,
                "Field accessors must be enumerated type");
  static_assert(size <= 64, "Size of register exceeds 64 bits");
  static_assert(detail::power_of_two(size),
                "Total register size not a power of two");
  static_assert(
      std::numeric_limits<std::underlying_type_t<field_accessor_type>>::max() >=
          sizeof...(fields),
      "Field accessor is not large enough to address all fields");

  type r;

  template <typename T>
  constexpr basic_register(
      T r,
      std::enable_if_t<!std::is_void<T>::value && std::is_pointer<E>::value> * =
          nullptr)
      : r(reinterpret_cast<type>(r)) {}

  template <typename T>
  basic_register(T r, std::enable_if_t<!std::is_void<T>::value &&
                                       !std::is_pointer<E>::value> * = nullptr)
      : r(r) {}

  template <field_accessor_type e>
  static void set(volatile value_type &r, value_type v) {
    constexpr auto sum =
        detail::sum(e, std::integer_sequence<std::size_t, 0, fields...>{});
    static_assert(sum < size, "Field bit offset exceeds limits");
    r |= v << sum;
  }

  template <field_accessor_type e>
  auto set(value_type v)
      -> std::enable_if_t<static_cast<std::size_t>(e) < sizeof...(fields) &&
                          std::is_pointer<E>::value> {
    set<e>(*r, v);
  }

  template <field_accessor_type e>
  auto set(value_type v)
      -> std::enable_if_t<static_cast<std::size_t>(e) < sizeof...(fields) &&
                          !std::is_pointer<E>::value> {
    set<e>(r, v);
  }
};
}

#endif
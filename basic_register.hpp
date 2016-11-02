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

template <typename E, std::size_t... fields>
constexpr std::size_t mask(E e, std::integer_sequence<std::size_t, fields...>) {
  constexpr std::size_t width[] = {fields...};
  return (1 << width[static_cast<std::size_t>(e)]) - 1;
}

template <typename T, typename... Ts> struct is_field {
  constexpr static bool value = is_field<T>::value && is_field<Ts...>::value;
};

template <typename T> struct is_field<T> {
  constexpr static bool value =
      std::is_integral<T>::value || std::is_enum<T>::value;
};

template <std::size_t N, typename T, typename... Ts> struct nth_type {
  using type = typename nth_type<N - 1, Ts...>::type;
};

template <typename T, typename... Ts> struct nth_type<0, T, Ts...> {
  using type = T;
};
}

template <typename E, std::size_t... fields> struct basic_register {
  constexpr static std::size_t size =
      detail::register_size(std::integer_sequence<std::size_t, fields...>{});
  using field_accessor_type = std::remove_pointer_t<E>;
  using value_type = detail::unit_t<size>;
  using type = std::conditional_t<!std::is_pointer<E>::value,
                                  volatile value_type, volatile value_type &>;

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
      : r(*new (r) value_type) {}

  template <typename T>
  constexpr basic_register(
      T r, std::enable_if_t<!std::is_void<T>::value &&
                            !std::is_pointer<E>::value> * = nullptr)
      : r(r) {}

  template <field_accessor_type e> auto set(value_type v) volatile {
    static_assert(static_cast<std::size_t>(e) < sizeof...(fields),
                  "Field accessor does not address field");
    constexpr auto offset =
        detail::sum(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(offset < size, "Field bit offset exceeds limits");
    constexpr auto m =
        detail::mask(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(detail::power_of_two(m + 1), "Mask is not all 1's");
    r |= ((v & m) << offset);
  }

  template <field_accessor_type e> auto get() volatile {
    static_assert(static_cast<std::size_t>(e) < sizeof...(fields),
                  "Field accessor does not address field");
    constexpr auto offset =
        detail::sum(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(offset < size, "Field bit offset exceeds limits");
    constexpr auto m =
        detail::mask(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(detail::power_of_two(m + 1), "Mask is not all 1's");
    return (r >> offset) & m;
  }

  template <field_accessor_type e> auto test(value_type v) volatile {
    static_assert(static_cast<std::size_t>(e) < sizeof...(fields),
                  "Field accessor does not address field");
    constexpr auto offset =
        detail::sum(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(offset < size, "Field bit offset exceeds limits");
    constexpr auto m =
        detail::mask(e, std::integer_sequence<std::size_t, fields...>{});
    static_assert(detail::power_of_two(m + 1), "Mask is not all 1's");
    return ((r >> offset) & m) == v;
  }
};

template <template <typename, std::size_t...> typename R, typename... Ts>
struct basic_typed_register {
  template <typename E, std::size_t... fields>
  struct typed_register : public R<E, fields...> {
    static_assert(sizeof...(fields) == sizeof...(Ts),
                  "Field types do not agree with number of fields");
    static_assert(detail::is_field<Ts...>::value,
                  "Field types are not integral or enumerations");

    template <typename T> typed_register(T t) : R<E, fields...>{t} {}

    template <E e>
    typename detail::nth_type<static_cast<std::size_t>(e), Ts...>::type
    get() volatile {
      return static_cast<
          typename detail::nth_type<static_cast<std::size_t>(e), Ts...>::type>(
          R<E, fields...>::template get<e>());
    }

    template <E e>
    void set(typename detail::nth_type<static_cast<std::size_t>(e), Ts...>::type
                 t) volatile {
      R<E, fields...>::template set<e>(
          static_cast<typename R<E, fields...>::value_type>(t));
    }

    template <E e>
    bool test(typename detail::nth_type<static_cast<std::size_t>(e),
                                        Ts...>::type t) volatile {
      return R<E, fields...>::template test<e>(
          static_cast<typename R<E, fields...>::value_type>(t));
    }
  };
};

template <typename E, std::size_t... fields>
using hardware_register = volatile basic_register<E, fields...>;
template <typename E, std::size_t... fields>
using memory_mapped_register = volatile basic_register<E *, fields...>;
}

#endif
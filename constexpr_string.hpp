#include <string>
#include <type_traits>
#include <utility>

namespace cexprstr_detail {
using namespace std;

constexpr size_t cstrlen(const char *s) {
  size_t l = 0;
  while (*s++ != 0)
    ++l;
  return l;
}

template <typename T> constexpr T *ccopy_n(const T *src, size_t len, T *dst) {
  while (len-- > 0)
    *(dst++) = *(src++);
  return dst;
}

template <typename T, size_t N> struct cexprstr {
  using value_type = T;
  T s[N] = {};
  constexpr cexprstr() = default;
  template <size_t M> constexpr cexprstr(const T (&s)[M]) {
    ccopy_n(s, N, this->s);
  }

  constexpr static size_t size() { return N; }
  constexpr const T operator[](size_t i) const { return s[i]; }
  operator string() { return string{s, s + N}; }
};

template <typename T, size_t N> cexprstr(const T (&s)[N])->cexprstr<T, N - 1>;

template <typename... S> constexpr common_type_t<S...> sum(S... s) {
  common_type_t<S...> ss = 0;
  for (auto i : {s...})
    ss += i;
  return ss;
}

template <typename T, size_t... N> constexpr auto concat(cexprstr<T, N>... s) {
  cexprstr<T, sum(N...)> str;
  auto first = str.s;
  for (auto cs : {make_pair(s.s, s.size())...})
    first = ccopy_n(cs.first, cs.second, first);
  return str;
}

template <typename T, size_t N, size_t M>
constexpr bool operator==(cexprstr<T, N> l, cexprstr<T, M> r) {
  return false;
}

template <typename T, size_t N, size_t M>
constexpr bool operator!=(cexprstr<T, N> l, cexprstr<T, M> r) {
  return true;
}

template <typename T, size_t N>
constexpr bool operator==(cexprstr<T, N> l, cexprstr<T, N> r) {
  for (size_t i = 0; i < N; ++i)
    if (l[i] != r[i])
      return false;
  return true;
}

template <typename T, size_t N>
constexpr bool operator!=(cexprstr<T, N> l, cexprstr<T, N> r) {
  return !(l == r);
}

template <typename T, size_t N, size_t M>
constexpr bool operator<(cexprstr<T, N> l, cexprstr<T, M> r) {
  for (size_t i = 0; i < min(N, M); ++i)
    if (l[i] < r[i])
      return true;
    else if (l[i] > r[i])
      return false;
  return N < M;
}

template <typename T, size_t N, size_t M>
constexpr bool operator>(cexprstr<T, N> l, cexprstr<T, M> r) {
  return r < l;
}

template <typename T, size_t N, size_t M>
constexpr bool operator<=(cexprstr<T, N> l, cexprstr<T, M> r) {
  return !(l > r);
}

template <typename T, size_t N, size_t M>
constexpr bool operator>=(cexprstr<T, N> l, cexprstr<T, M> r) {
  return !(l < r);
}

template <typename T, size_t N, size_t M>
constexpr auto operator+(cexprstr<T, N> self, cexprstr<T, M> other) {
  return concat(self, other);
}

template <typename T, size_t N, size_t M>
constexpr auto operator+(cexprstr<T, N> self, const T (&s)[M]) {
  return concat(self, cexprstr{s});
}

template <typename T, size_t N, size_t M>
constexpr auto operator+(const T (&s)[N], cexprstr<T, M> other) {
  return concat(cexprstr{s}, other);
}

template <typename T, size_t N>
ostream &operator<<(ostream &out, cexprstr<T, N> s) {
  for (auto c : s.s)
    out << c;
  return out;
}

template <typename T> constexpr void cswap(T *l, T *r) {
  if (l == r)
    return;
  const T temp = *l;
  *l = *r;
  *r = temp;
}

template <typename T> constexpr T *cpartition(T *first, T *last) {
  if (first == last)
    return first;
  const auto midval = *(first + (last - first - 1) / 2);

  while (first != last) {
    while (first != last && (*first < midval))
      ++first;
    while (first != last && (midval < *(last - 1)))
      --last;
    if (first == last)
      break;
    cswap(first, last - 1);
    if (++first != last)
      --last;
  }

  return first;
}

template <typename T> constexpr void csort(T *first, T *last) {
  if (last - first < 2)
    return;

  auto midpoint = cpartition(first, last);
  csort(first, midpoint);
  csort(midpoint, last);
}

template <typename T> constexpr T *crotate(T *first, T *mid, T *last) {
  auto new_mid = last - (mid - first);
  while (first != mid && mid != last) {
    auto next = mid;
    while (first != mid && next != last)
      cswap(first++, next++);
    if (first == mid)
      mid = next;
  }
  return new_mid;
}

template <typename T> constexpr T *cunique(T *first, T *last) {
  auto next = first;
  if (next != last)
    ++next;
  while (next != last) {
    while (next != last && *next == *first)
      ++next;
    if (next != last)
      cswap(++first, next++);
  }
  if (first != last)
    ++first;
  return first;
}

template <typename... T> constexpr size_t num_unique(T... t) {
  common_type_t<T...> size_order[] = {t...};
  csort(size_order, size_order + sizeof...(T));
  return cunique(size_order, size_order + sizeof...(T)) - size_order;
}

template <size_t N, typename... T> constexpr auto all_unique(T... t) {
  cexprstr<common_type_t<T...>, N> arr;
  common_type_t<T...> size_order[] = {t...};
  csort(size_order, size_order + sizeof...(T));
  cunique(size_order, size_order + sizeof...(T));
  ccopy_n(size_order, N, arr.s);
  return arr;
}

template <typename T, typename... U> constexpr size_t count(T t, U... u) {
  return sum((t == u ? 1 : 0)...);
}

template <typename... T> struct Tuple : T... {
  constexpr Tuple() = default;
  constexpr Tuple(T... t) : T(t)... {}
  constexpr static size_t buckets() { return sizeof...(T); }
  constexpr static size_t size() { return sum((T::size())...); }
};

template <typename... T> ostream &operator<<(ostream &out, Tuple<T...> t) {
  auto v = {(out << static_cast<T>(t) << '\n', 0)...};
  (void)v;
  return out;
}

template <typename T, typename U, typename... V>
constexpr auto bucket(U u, V... v) {
  if constexpr (is_same<T, U>::value) {
    U w[] = {u};
    cexprstr<U, 1> c{w};
    if constexpr (sizeof...(V) == 0) {
      return c;
    } else {
      using B = decltype(bucket<T>(v...));
      if constexpr (is_same<B, nullptr_t>::value)
        return c;
      else if constexpr (!is_same<typename B::value_type, T>::value)
        return c;
      else
        return c + bucket<T>(v...);
    }
  } else {
    if constexpr (sizeof...(V) == 0)
      return nullptr;
    else
      return bucket<T>(v...);
  }
}

template <typename T, typename... U> constexpr auto sorted_bucket(U... u) {
  auto b = bucket<T>(u...);
  csort(b.s, b.s + b.size());
  return b;
}

template <typename... T, typename... U> constexpr auto histogramify(U... u) {
  return Tuple{sorted_bucket<typename T::value_type>(u...)...};
}

template <size_t... I, typename T, size_t... N>
constexpr auto csorted(index_sequence<I...>, cexprstr<T, N>... s) {
  constexpr auto all = all_unique<sizeof...(I)>(N...);
  return histogramify<
      cexprstr<cexprstr<T, all.s[I]>, count(all.s[I], N...)>...>(s...);
}

template <typename T, size_t... N> constexpr auto csorted(cexprstr<T, N>... s) {
  return csorted(make_index_sequence<num_unique(N...)>{}, s...);
}

template <typename T>
constexpr const T *clower_bound(const T *first, const T *last, T t) {
  while (first != last) {
    auto mid = first + (last - first) / 2;
    if (*mid < t)
      first = ++mid;
    else
      last = mid;
  }
  return first;
}

template <typename T>
constexpr bool cbinary_search(const T *first, const T *last, T t) {
  auto lower = clower_bound(first, last, t);
  return lower != last && !(t < *lower);
}

template <typename T, size_t N>
constexpr bool cbinary_search(T t, cexprstr<T, N> c) {
  return cbinary_search(c.s, c.s + N, t);
}

template <typename... T, size_t... N, typename U, size_t M>
constexpr bool cbinary_search(Tuple<cexprstr<T, N>...> c, cexprstr<U, M> u) {
  return cbinary_search(u, c);
}

template <typename... T> constexpr size_t index_of(...) {
  static_assert(sizeof...(T) == 0);
  return -1;
}

template <typename T, size_t N>
constexpr size_t index_of(T t, cexprstr<T, N> c) {
  auto low = clower_bound(c.s, c.s + N, t);
  if (low == c.s + N || t < *low)
    return -1;
  return low - c.s;
}

template <typename... T, size_t... N>
constexpr size_t count_to(Tuple<cexprstr<T, N>...> t, size_t count) {
  constexpr size_t n[] = {N...};
  size_t s = 0;
  for (size_t i = 0; i < count; ++i)
    s += n[i];
  return s;
}

template <typename... T, size_t... N, typename U, size_t M>
constexpr size_t group_of(Tuple<cexprstr<T, N>...> t, cexprstr<U, M> c) {
  constexpr size_t n[] = {T::size()...};
  return clower_bound(n, n + sizeof...(N), M) - n;
}

template <typename... T, size_t... N, typename U, size_t M>
constexpr size_t index_of(Tuple<cexprstr<T, N>...> t, cexprstr<U, M> c) {
  auto ind = index_of(c, t);
  if (ind == -1)
    return ind;
  return count_to(t, group_of(t, c)) + ind;
}

template <size_t I, size_t C, typename T, size_t N, typename... U, size_t... M>
constexpr auto at_index(cexprstr<T, N> t, cexprstr<U, M>... u) {
  constexpr auto offset = I - C;
  constexpr auto count = C + N;

  if constexpr (I < count && !(I < C))
    return t.s[offset];
  else if constexpr (sizeof...(U) > 0)
    return at_index<I, count>(u...);
  else
    return nullptr;
}

template <size_t I, typename... T, size_t... N>
constexpr auto at_index(Tuple<cexprstr<T, N>...> t) {
  return at_index<I, 0>(static_cast<cexprstr<T, N>>(t)...);
}

template <size_t N> struct EIdx { constexpr static size_t value = N; };

template <typename T> struct Enum : T {
  template <typename... U> constexpr Enum(U... u) : T(csorted(u...)) {}
  template <size_t... N>
  constexpr Enum(const char (&... s)[N]) : T(csorted(cexprstr{s}...)) {}

  template <size_t N> constexpr size_t operator[](const char (&s)[N]) const {
    return index_of(static_cast<T>(*this), cexprstr{s});
  }

  template <typename U, size_t N>
  constexpr size_t operator[](cexprstr<U, N> s) const {
    return index_of(static_cast<T>(*this), s);
  }

  template <size_t I> constexpr auto at() const {
    return at_index<I>(static_cast<T>(*this));
  }

  template <size_t I> constexpr auto operator[](EIdx<I>) const {
    return at<I>();
  }
};

template <typename T, size_t... U>
Enum(cexprstr<T, U>... u)->Enum<decltype(csorted(u...))>;

template <size_t... N>
Enum(const char (&... s)[N])->Enum<decltype(csorted(cexprstr{s}...))>;
} // namespace cexprstr_detail

using cexprstr_detail::Enum;
using cexprstr_detail::cexprstr;

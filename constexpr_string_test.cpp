#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <typeinfo>

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
  constexpr const T &operator[](size_t i) const { return s[i]; }
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
    if (!(t < *first))
      return first;
    auto mid = first + (last - first - 1) / 2;
    if (t < *mid)
      first = ++mid;
    else
      last = mid;
  }
  return last;
}

template <typename T>
constexpr bool cbinary_search(const T *first, const T *last, T t) {
  auto lower = clower_bound(first, last, t);
  return lower != last && *lower == t;
}

template <typename T, size_t N>
constexpr bool cbinary_search(T t, cexprstr<T, N> c) {
  return cbinary_search(c.s, c.s + N, t);
}

template <typename... T, size_t... N, typename U, size_t M>
constexpr bool cbinary_search(Tuple<cexprstr<T, N>...> c, cexprstr<U, M> u) {
  return cbinary_search(u, c);
}

int main() {
  constexpr cexprstr hello = "Hello", world = "World";
  constexpr cexprstr one = "one";
  constexpr cexprstr eleven = "eleven";
  constexpr cexprstr twenty_one = "twenty one";
  constexpr cexprstr thirty_one = "thirty one";
  constexpr cexprstr forty_one = "forty one";
  constexpr cexprstr hundred_and_one = "hundred and one";
  constexpr auto s = hello + world;
  cout << integral_constant<size_t, s.size()>::value << '\n';
  cout << s << '\n';
  constexpr auto ss = concat(hello, world, world, hello);
  constexpr auto sss =
      "!" + hello + " " + world + " " + world + " " + hello + "!";
  cout << integral_constant<size_t, ss.size()>::value << ':' << ss << '\n';
  cout << integral_constant<size_t, sss.size()>::value << ':' << sss << '\n';
  cout << integral_constant<char, sss[3]>::value << '\n';

  static_assert(hello == hello);
  static_assert(hello <= hello);
  static_assert(hello >= hello);
  static_assert(hello != world);
  static_assert(hello <= world);
  static_assert(world > hello);
  static_assert(world >= hello);
  static_assert(concat(hello, world) == hello + world);
  static_assert(hello < hello + "world");

  cout << integral_constant<size_t, cstrlen("hello")>::value << '\n';

  size_t unsorted[] = {3, 1, 4, 1, 5, 9, 2, 7, 5, 3, 5, 8, 9, 6, 9};
  csort(unsorted, unsorted + extent<decltype(unsorted)>{});
  for (auto s : unsorted)
    cout << s << ' ';
  cout << '\n';
  auto mid = cunique(unsorted, unsorted + extent<decltype(unsorted)>{});
  copy(unsorted, mid, ostream_iterator<size_t>(cout, ","));
  cout << '\n';
  copy(unsorted, unsorted + extent<decltype(unsorted)>{},
       ostream_iterator<size_t>(cout, ","));
  cout << '\n';
  cout << "Num PI unique: "
       << integral_constant<size_t, num_unique(3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5,
                                               8, 9, 7, 9)>::value
       << '\n';
  cout << "All PI unique: "
       << all_unique<num_unique(3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9)>(
              3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9)
       << '\n';
  cout << "Number of 5s: "
       << integral_constant<size_t, count(5, 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8,
                                          9, 7, 9)>::value
       << '\n';

  size_t same[] = {5, 5, 5};
  csort(same, same + extent<decltype(same)>{});
  for (auto s : same)
    cout << s << ' ';
  cout << '\n';

  for (auto i = 0, end = 11; i < end; ++i) {
    size_t unrotated[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto mid = crotate(unrotated, unrotated + i, unrotated + end);
    cout << "Mid: " << i << ", new point: " << (mid - unrotated) << " -> ";
    for (auto r : unrotated)
      cout << r << ' ';
    cout << '\n';
  }

  constexpr auto sorted =
      csorted(forty_one, hundred_and_one, world, hello, one, eleven, twenty_one,
              thirty_one, hello + world);
  cout << "Sorted:\n" << sorted << '\n';
  static_assert(cbinary_search(sorted, forty_one));
  static_assert(cbinary_search(sorted, hundred_and_one));
  static_assert(!cbinary_search(sorted, cexprstr{"hundred and onf"}));
  static_assert(cbinary_search(sorted, hello + world));
  static_assert(cbinary_search(sorted, cexprstr{"one"}));
  static_assert(!cbinary_search(sorted, cexprstr{"aaa"}));
  static_assert(!cbinary_search(sorted, cexprstr{"two"}));

  return 0;
}
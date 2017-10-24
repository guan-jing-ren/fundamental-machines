#include <algorithm>
#include <iomanip>
#include <iostream>
#include <tuple>

using namespace std;

constexpr size_t cstrlen(const char *s) {
  size_t l = 0;
  while (*s++ != 0)
    ++l;
  return l;
}

constexpr char *ccopy_n(const char *src, size_t len, char *dst) {
  while (len-- > 0)
    *(dst++) = *(src++);
  return dst;
}

template <size_t N> struct cexprstr {
  char s[N] = {};
  constexpr cexprstr() = default;
  template <size_t M> constexpr cexprstr(const char (&s)[M]) {
    ccopy_n(s, N, this->s);
  }
  constexpr size_t size() const { return N; }
  constexpr const char &operator[](size_t i) const { return s[i]; }
  operator string() { return string{s, s + N}; }
};

template <size_t N> cexprstr(const char (&s)[N])->cexprstr<N - 1>;
template <char... C> constexpr cexprstr<sizeof...(C)> operator""_css() {
  char s[] = {C...};
  return cexprstr<sizeof...(C)>{s};
}

template <typename... S> constexpr common_type_t<S...> sum(S... s) {
  common_type_t<S...> ss = 0;
  for (auto i : {s...})
    ss += i;
  return ss;
}

template <size_t... N> constexpr auto concat(cexprstr<N>... s) {
  cexprstr<sum(N...)> str;
  auto first = str.s;
  for (auto cs : {make_pair(s.s, s.size())...})
    first = ccopy_n(cs.first, cs.second, first);
  return str;
}

template <size_t N, size_t M>
constexpr bool operator==(cexprstr<N> l, cexprstr<M> r) {
  return false;
}

template <size_t N, size_t M>
constexpr bool operator!=(cexprstr<N> l, cexprstr<M> r) {
  return true;
}

template <size_t N> constexpr bool operator==(cexprstr<N> l, cexprstr<N> r) {
  for (size_t i = 0; i < N; ++i)
    if (l[i] != r[i])
      return false;
  return true;
}

template <size_t N> constexpr bool operator!=(cexprstr<N> l, cexprstr<N> r) {
  return !(l == r);
}

template <size_t N, size_t M>
constexpr bool operator<(cexprstr<N> l, cexprstr<M> r) {
  for (size_t i = 0; i < min(N, M); ++i)
    if (l[i] < r[i])
      return true;
    else if (l[i] > r[i])
      return false;
  return N < M;
}

template <size_t N, size_t M>
constexpr bool operator>(cexprstr<N> l, cexprstr<M> r) {
  return r < l;
}

template <size_t N, size_t M>
constexpr bool operator<=(cexprstr<N> l, cexprstr<M> r) {
  return !(l > r);
}

template <size_t N, size_t M>
constexpr bool operator>=(cexprstr<N> l, cexprstr<M> r) {
  return !(l < r);
}

template <size_t N, size_t M>
constexpr auto operator+(cexprstr<N> self, cexprstr<M> other) {
  return concat(self, other);
}

template <size_t N, size_t M>
constexpr auto operator+(cexprstr<N> self, const char (&s)[M]) {
  return concat(self, cexprstr{s});
}

template <size_t N, size_t M>
constexpr auto operator+(const char (&s)[N], cexprstr<M> other) {
  return concat(cexprstr{s}, other);
}

template <size_t N> ostream &operator<<(ostream &out, cexprstr<N> s) {
  return out << setw(N) << s.s << setw(0);
}

int main() {
  constexpr cexprstr hello = "Hello", world = "World";
  constexpr auto s = hello + world;
  cout << integral_constant<size_t, s.size()>::value << '\n';
  cout << s << '\n';
  constexpr auto ss = concat(hello, 0_css, world, world, hello);
  constexpr auto sss =
      "!" + hello + " " + world + " " + world + " " + hello + "!";
  cout << integral_constant<size_t, ss.size()>::value << ':' << ss << '\n';
  cout << integral_constant<size_t, sss.size()>::value << ':' << sss << '\n';
  cout << integral_constant<char, sss[3]>::value << '\n';

  static_assert(hello == hello, "hello does not equate itself!");
  static_assert(hello <= hello, "hello not less than or equal itself!");
  static_assert(hello >= hello, "hello not greater than or equal itself!");
  static_assert(hello != world, "hello equates with world!");
  static_assert(hello < world, "hello not less than world!");
  static_assert(hello <= world, "hello not less than or equal to world!");
  static_assert(world > hello, "world not greater than hello!");
  static_assert(world >= hello, "world not greater than or equal to hello!");

  static_assert(concat(hello, world) == hello + world,
                "concat not equal to sum!");
  static_assert(hello < hello + "world", "hello not less than helloworld");

  cout << integral_constant<size_t, cstrlen("hello")>::value << '\n';

  return 0;
}
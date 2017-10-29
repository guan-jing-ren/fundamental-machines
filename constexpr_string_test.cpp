#include "constexpr_string.hpp"

#include <iostream>
#include <iterator>

using namespace std;
using namespace cexprstr_detail;

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

  size_t unsorted[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
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

  cout << "Count 0: " << integral_constant<size_t, count_to(sorted, 1)>::value
       << '\n';
  cout << "Count 1: " << integral_constant<size_t, count_to(sorted, 2)>::value
       << '\n';
  cout << "Count 2: " << integral_constant<size_t, count_to(sorted, 3)>::value
       << '\n';
  cout << "Count 3: " << integral_constant<size_t, count_to(sorted, 4)>::value
       << '\n';
  cout << "Count 4: " << integral_constant<size_t, count_to(sorted, 5)>::value
       << '\n';
  cout << "Count 5: " << integral_constant<size_t, count_to(sorted, 6)>::value
       << '\n';

  cout << "Group of " << forty_one << ": "
       << integral_constant<size_t, group_of(sorted, forty_one)>::value << '\n';
  cout << "Group of " << twenty_one << ": "
       << integral_constant<size_t, group_of(sorted, twenty_one)>::value
       << '\n';

  cout << "Index of " << forty_one << ": "
       << integral_constant<size_t, index_of(sorted, forty_one)>::value << '\n';
  cout << "Index of " << twenty_one << ": "
       << integral_constant<size_t, index_of(sorted, twenty_one)>::value
       << '\n';

  constexpr Enum enums{eleven, one};
  constexpr Enum what_in_the_world{"what", "in", "the", "World"};

  cout << enums << '\n';
  cout << what_in_the_world << '\n';

  cout << integral_constant<size_t, enums["eleven"]>::value << '\n';
  cout << integral_constant<size_t, enums["two"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world["two"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world["what"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world["in"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world["the"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world["world"]>::value << '\n';
  cout << integral_constant<size_t, what_in_the_world[world]>::value << '\n';

  constexpr auto c_at0 = what_in_the_world[EIdx<0>{}];
  constexpr auto c_at1 = what_in_the_world[EIdx<1>{}];
  constexpr auto c_at2 = what_in_the_world[EIdx<2>{}];
  constexpr auto c_at3 = what_in_the_world[EIdx<3>{}];

  cout << "At index 0: " << c_at0 << '\n';
  cout << "At index 1: " << c_at1 << '\n';
  cout << "At index 2: " << c_at2 << '\n';
  cout << "At index 3: " << c_at3 << '\n';
  constexpr auto eidx4 = what_in_the_world[EIdx<4>{}];
  static_assert(eidx4 == nullptr);

  return 0;
}
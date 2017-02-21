// Example program
#include <iostream>
#include <string>
#include <vector>
#include <boost/preprocessor.hpp>

struct tag {
  std::vector<tag> tags;
};

#define MEMDECL(r, data, i, elem) elem;
#define AS_MEMBERS(...)                                                        \
  BOOST_PP_SEQ_FOR_EACH_I(MEMDECL, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define ATTDECL(r, data, i, elem) std::string elem;
#define ATTR(...)                                                              \
  BOOST_PP_SEQ_FOR_EACH_I(ATTDECL, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__));

#define DEFTAG(t, ...)                                                         \
  struct t {                                                                   \
    AS_MEMBERS(__VA_ARGS__);                                                   \
  }

#define SUBTAG(t, ...) DEFTAG(t, __VA_ARGS__) t
#define SUBTAGL(t, ...)                                                        \
  DEFTAG(t##_t, __VA_ARGS__);                                                  \
  std::vector<t##_t> t

DEFTAG(mystruct,                          //
       SUBTAG(myinnerstruct,              //
              ATTR(a = "poiu",            //
                   b = ";lkj",            //
                   c = ".,mn"),           //
              SUBTAGL(thirdlevel,         //
                      ATTR(n = "blah"))), //
       ATTR(x = "asdf",                   //
            y = "qwer",                   //
            z = "zxcv")                   //
       );

int main() {
  tag tg;
  fill_n(back_inserter(tg.tags), 20, tag{});
  std::cout << tg.tags.size() << '\n';

  mystruct mystruct;
  std::cout << mystruct.x << ' ' << mystruct.y << ' ' << mystruct.z << '\n';
  std::cout << mystruct.myinnerstruct.a << ' ' << mystruct.myinnerstruct.b
            << ' ' << mystruct.myinnerstruct.c << '\n';
  mystruct.myinnerstruct.thirdlevel.push_back({});
  std::cout << mystruct.myinnerstruct.thirdlevel.front().n << '\n';

  return 0;
}

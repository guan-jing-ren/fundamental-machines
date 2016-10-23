#ifndef _TEST_UTILS_HPP_
#define _TEST_UTILS_HPP_

#include <stdexcept>

namespace fm {
struct test_error : public std::logic_error {
  using std::logic_error::logic_error;
};
}

#endif
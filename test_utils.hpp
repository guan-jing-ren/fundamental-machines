#include <stdexcept>

namespace fm {
struct test_error : public std::logic_error {
  using std::logic_error::logic_error;
};
}
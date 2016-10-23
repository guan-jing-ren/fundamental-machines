#include <cstdio>
#include <cstdarg>
#include <type_traits>
#include "test_utils.hpp"
#include "basic_resource.hpp"
#include "file_resource.hpp"

int main() {
  fm::file_resource other{"non-existent", "r"};
  auto file = std::fopen("non-existent-steal", "w");
  if (!file)
    throw fm::test_error{"File did not open properly"};
  auto other_steal = fm::file_resource::steal(file);
  if (file)
    throw fm::test_error{"File was not stolen"};
  if (!other_steal)
    throw fm::test_error{"Underlying resource not handled"};
  std::C::FILE move_steal = std::move(other_steal);
  if (other_steal || !move_steal)
    throw fm::test_error{"Underlying resource not moved"};
  return 0;
}
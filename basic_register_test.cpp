#include "basic_register.hpp"
#include "test_utils.hpp"

enum class RegField { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH };

fm::detail::unit_t<32> r;
int main() {
  fm::basic_register<RegField *, 2, 5, 4, 7, 32 - 18> reg = &r;
  reg.set<RegField::SECOND>(63);
  reg.set<RegField::FIFTH>(7);
  fm::basic_register<RegField, 2, 5, 4, 7, 32 - 18> reg2 = 0;
  reg2.set<RegField::SECOND>(63);
  reg2.set<RegField::FIFTH>(7);

  if (r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Memory mapped register test failed"};
  if (reg2.r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Value register test failed"};
  return 0;
}
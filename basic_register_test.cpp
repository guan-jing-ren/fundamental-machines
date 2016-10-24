#include "basic_register.hpp"

enum class RegField { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH };

int main() {
  fm::basic_register<RegField *, 2, 5, 4, 7, 32 - 18> reg = 0x8843229;
  reg.set<RegField::FIRST>(1);
  fm::basic_register<RegField, 2, 5, 4, 7, 32 - 18> reg2 = 0x8843229u;
  reg2.set<RegField::SECOND>(2);
  reg2.set<RegField::FIFTH>(2);
  return 0;
}
#include "basic_register.hpp"

enum class RegField { FIRST };

int main() {
  fm::basic_register<RegField *, 2, 5, 4, 7, 32 - 18> reg = 0x8843229;
  reg.set<RegField::FIRST>(1);
  fm::basic_register<RegField, 2, 5, 4, 7, 32 - 18> reg2 = 0x8843229u;
  reg2.set<RegField::FIRST>(2);
  return 0;
}
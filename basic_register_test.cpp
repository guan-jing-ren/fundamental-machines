#include "basic_register.hpp"
#include <iostream>
#include <bitset>

enum class RegField { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH };

fm::detail::unit_t<32> r;
int main() {
  fm::basic_register<RegField *, 2, 5, 4, 7, 32 - 18> reg = &r;
  reg.set<RegField::SECOND>(63);
  reg.set<RegField::FIFTH>(7);
  fm::basic_register<RegField, 2, 5, 4, 7, 32 - 18> reg2 = 0;
  reg2.set<RegField::SECOND>(63);
  reg2.set<RegField::FIFTH>(7);

  std::cout << std::bitset<32>{*reg.r}.to_string() << std::endl;
  std::cout << std::bitset<32>{reg2.r}.to_string() << std::endl;
  return 0;
}
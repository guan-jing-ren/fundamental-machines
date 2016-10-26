#include "basic_register.hpp"
#include "test_utils.hpp"
#include <iostream>
#include <bitset>

enum class RegField { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH };

fm::detail::unit_t<32> r = 0;
int main() {
  fm::memory_mapped_register<RegField, 2, 5, 4, 7, 32 - 18> reg = &r;
  reg.set<RegField::SECOND>(63);
  reg.set<RegField::FIFTH>(7);
  fm::hardware_register<RegField, 2, 5, 4, 7, 32 - 18> reg2 = 0;
  reg2.set<RegField::SECOND>(63);
  reg2.set<RegField::FIFTH>(7);

  if (r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Memory mapped register test failed"};
  if (reg2.r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Value register test failed"};
  if (reg.get<RegField::SECOND>() != 0b11111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg.get<RegField::FIFTH>() != 0b111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg2.get<RegField::SECOND>() != 0b11111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg2.get<RegField::FIFTH>() != 0b111)
    throw fm::test_error{"Field retrieval incorrect value"};

  reg.set<RegField::THIRD>(13);
  reg2.set<RegField::THIRD>(13);

  if (!reg.test<RegField::THIRD>(0b1101))
    throw fm::test_error{"Field test failed"};
  if (!reg2.test<RegField::THIRD>(0b1101))
    throw fm::test_error{"Field test failed"};

  r = 0;
  fm::basic_typed_register<fm::hardware_register, int, int, int, int,
                           int>::typed_register<RegField, 2, 5, 4, 7, 32 - 18>
      reg3 = 0;
  fm::basic_typed_register<fm::memory_mapped_register, int, int, int, int,
                           int>::typed_register<RegField, 2, 5, 4, 7, 32 - 18>
      reg4 = &r;

  reg3.set<RegField::SECOND>(63);
  reg3.set<RegField::FIFTH>(7);
  reg4.set<RegField::SECOND>(63);
  reg4.set<RegField::FIFTH>(7);

  if (r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Memory mapped register test failed"};
  if (reg3.r != 0b00000000000111000000000001111100)
    throw fm::test_error{"Value register test failed"};
  if (reg4.get<RegField::SECOND>() != 0b11111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg4.get<RegField::FIFTH>() != 0b111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg3.get<RegField::SECOND>() != 0b11111)
    throw fm::test_error{"Field retrieval incorrect value"};
  if (reg3.get<RegField::FIFTH>() != 0b111)
    throw fm::test_error{"Field retrieval incorrect value"};

  reg4.set<RegField::THIRD>(13);
  reg3.set<RegField::THIRD>(13);

  if (!reg4.test<RegField::THIRD>(0b1101))
    throw fm::test_error{"Field test failed"};
  if (!reg3.test<RegField::THIRD>(0b1101))
    throw fm::test_error{"Field test failed"};

  return 0;
}
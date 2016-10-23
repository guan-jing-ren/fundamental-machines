#include "basic_register.hpp"

enum class RegField {

};

int main() {
  fm::basic_register<RegField, 2, 5, 4, 7, 32 - 18> reg;
  (void)reg;
  return 0;
}
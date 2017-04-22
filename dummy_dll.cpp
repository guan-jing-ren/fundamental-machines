#include <cstdlib>
#include <ctime>
#include <string>

extern "C" {
int return_an_integer() {
  std::srand(std::time(nullptr));
  return std::rand();
}

std::string magic_value = "magic string";
}
#include "basic_plugin.hpp"

#include <iostream>

int main() {
  basic_symbol<int()> second_return_an_integer;
  {
    basic_plugin plugin{"./libdummy.so"};
    auto return_an_integer = plugin.sym<int()>("return_an_integer");
    std::clog << std::boolalpha << (return_an_integer ? true : false) << '\n';
    std::clog << "Returning an integer: " << return_an_integer() << '\n';
    std::clog << "Returning an integer: " << return_an_integer() << '\n';
    std::clog << "Returning an integer: " << return_an_integer() << '\n';
    std::clog << "Returning an integer: " << return_an_integer() << '\n';
    second_return_an_integer = return_an_integer;

    try {
      auto dont_return_an_integer =
          plugin.sym<void()>("dont_return_an_integer");
      std::clog << std::boolalpha << (dont_return_an_integer ? true : false)
                << '\n';
    } catch (...) {
      std::clog << "No symbol\n";
    }

    std::clog << static_cast<std::string>(
                     plugin.sym<std::string>("magic_value"))
              << '\n';
  }

  std::clog << "Returning an integer: " << second_return_an_integer() << '\n';
  std::clog << "Returning an integer: " << second_return_an_integer() << '\n';
  std::clog << "Returning an integer: " << second_return_an_integer() << '\n';
  std::clog << "Returning an integer: " << second_return_an_integer() << '\n';
  return 0;
}
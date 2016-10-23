#include <cstdio>
#include "basic_resource.hpp"

namespace fm {
struct FileTraits : public fm::resource_traits<std::FILE *> {
  static std::FILE *constructor(const char *name, const char *mode) {
    return std::fopen(name, mode);
  }

  static void destructor(std::FILE *f) { std::fclose(f); }
};

using file_resource = basic_resource<std::FILE *, FileTraits>;
}

int main() {
  auto file = std::fopen("non-existent", "r");

  fm::file_resource other{"non-existent", "r"};
  auto other_steal = fm::file_resource::steal(file);
  return 0;
}
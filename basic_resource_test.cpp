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
  auto file_resource = fm::basic_resource<std::FILE *>::steal(file);
  fclose(file_resource);

  fm::basic_resource<std::FILE *> resource;

  fm::file_resource other{"non-existent", "r"};
  return 0;
}
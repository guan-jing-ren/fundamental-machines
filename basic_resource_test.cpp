#include <cstdio>
#include "basic_resource.hpp"

int main() {
  fm::basic_resource<FILE*> file_resource;
  (void) file_resource;
  return 0;
}
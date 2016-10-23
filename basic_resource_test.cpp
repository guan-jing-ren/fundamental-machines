#include <cstdio>
#include <type_traits>
#include "test_utils.hpp"
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

namespace std {
namespace C {
struct FILE : public fm::file_resource {
  using fm::file_resource::file_resource;

  FILE &reopen(const char *mode) {
    t = std::freopen(nullptr, mode, *this);
    return *this;
  }

  FILE &reopen(const char *name, const char *mode) {
    t = std::freopen(name, mode, *this);
    return *this;
  }

  FILE &flush() {
    std::fflush(t);
    return *this;
  }

  FILE &wide(bool wide) {
    std::fwide(*this, wide);
    return *this;
  }

  FILE &setbuf(char *buffer) {
    std::setbuf(*this, buffer);
    return *this;
  }

  template <std::size_t N> FILE &setbuf(char (&buffer)[N]) {
    static_assert(N <= BUFSIZ, "Provided buffer is not within limits");
    std::setbuf(*this, buffer);
    return *this;
  }

  template <int mode> FILE &setvbuf(char *buffer, std::size_t size) {
    static_assert(mode == _IOFBF || mode == _IOLBF || mode == _IONBF,
                  "Provided buffering mode is invalid, choose one of _IOFBF, "
                  "_IOLBF, _IONBF");
    std::setvbuf(*this, buffer, mode, size);
    return *this;
  }

  template <int mode, std::size_t N> FILE &setvbuf(char (&buffer)[N]) {
    static_assert(mode == _IOFBF || mode == _IOLBF || mode == _IONBF,
                  "Provided buffering mode is invalid, choose one of _IOFBF, "
                  "_IOLBF, _IONBF");
    static_assert(N <= BUFSIZ, "Provided buffer is not within limits");
    std::setvbuf(*this, buffer, mode, N);
    return *this;
  }

  template <typename T> auto read(T *buffer, std::size_t count) {
    static_assert(std::is_trivially_copyable<T>{},
                  "Buffer type is not trivially copyable");
    return std::fread(buffer, sizeof(T), count, *this);
  }

  template <typename T, std::size_t N> auto read(T (&buffer)[N]) {
    static_assert(std::is_trivially_copyable<T>{},
                  "Buffer type is not trivially copyable");
    return std::fread(buffer, sizeof(T), N, *this);
  }

  template <typename T> auto write(T *buffer, std::size_t count) {
    static_assert(std::is_trivially_copyable<T>{},
                  "Buffer type is not trivially copyable");
    return std::fwrite(buffer, sizeof(T), count, *this);
  }

  template <typename T, std::size_t N> auto write(T (&buffer)[N]) {
    static_assert(std::is_trivially_copyable<T>{},
                  "Buffer type is not trivially copyable");
    return std::fwrite(buffer, sizeof(T), N, *this);
  }
};
}
}

int main() {
  fm::file_resource other{"non-existent", "r"};
  auto file = std::fopen("non-existent-steal", "w");
  if (!file)
    throw fm::test_error{"File did not open properly"};
  auto other_steal = fm::file_resource::steal(file);
  if (file)
    throw fm::test_error{"File was not stolen"};
  if (!other_steal)
    throw fm::test_error{"Underlying resource not handled"};
  std::C::FILE move_steal = std::move(other_steal);
  if (other_steal || !move_steal)
    throw fm::test_error{"Underlying resource not moved"};
  return 0;
}
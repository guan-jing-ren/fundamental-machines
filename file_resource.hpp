#ifndef _FILE_RESOURCE_H_
#define _FILE_RESOURCE_H_

#include "basic_resource.hpp"

namespace fm {

// The provided resource_trait for FILE* objects, using the basic fopen and
// fclose API.
struct FileTraits : public fm::resource_traits<std::FILE *> {
  static std::FILE *constructor(const char *name, const char *mode) {
    return std::fopen(name, mode);
  }

  static void destructor(std::FILE *f) { std::fclose(f); }
};

using file_resource = basic_resource<std::FILE *, FileTraits>;
}

namespace std {
// To avoid confusion with the regular C types, put it in a new namespace for
// specifically C updates.
namespace C {

// For FILE* C API functions that either return a FILE* or void, return a
// reference to itself. The 'f' prefix is omitted for all API wrappers. As much
// as possible, use overloading to replace teh C APIs that do the same thing for
// different types.
struct FILE : public fm::file_resource {
  using fm::file_resource::file_resource;

  // freopen can handle a nullptr name, so this wrapper provides the explicit
  // option to omit it.
  FILE &reopen(const char *mode) {
    std::freopen(nullptr, mode, *this);
    return *this;
  }

  FILE &reopen(const char *name, const char *mode) {
    std::freopen(name, mode, *this);
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

  // The standard actually mandates the contents of the buffer is trivially
  // copyable. So this FILE* wrapper takes this chance to statically assert this
  // requirement through type deduction rather than take a void* like in the C
  // API.
  template <typename T> auto read(T *buffer, std::size_t count) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Buffer type is not trivially copyable");
    return std::fread(buffer, sizeof(T), count, *this);
  }

  template <typename T, std::size_t N> auto read(T (&buffer)[N]) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Buffer type is not trivially copyable");
    return std::fread(buffer, sizeof(T), N, *this);
  }

  template <typename T> auto write(T *buffer, std::size_t count) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Buffer type is not trivially copyable");
    return std::fwrite(buffer, sizeof(T), count, *this);
  }

  template <typename T, std::size_t N> auto write(T (&buffer)[N]) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "Buffer type is not trivially copyable");
    return std::fwrite(buffer, sizeof(T), N, *this);
  }

  auto get() { return std::fgetc(*this); }
  auto put(unsigned char c) { return std::fputc(c, *this); }

  auto get(char *buffer, std::size_t length) {
    return std::fgets(buffer, length, *this);
  }
  template <std::size_t N> auto get(char (&buffer)[N]) {
    return std::fgets(buffer, N, *this);
  }
  FILE &put(const char *buffer) {
    std::fputs(buffer, *this);
    return *this;
  }
  auto unget(int c) { return std::ungetc(c, *this); }

  auto getw() { return std::fgetwc(*this); }
  auto put(wchar_t c) { return std::fputwc(c, *this); }

  auto get(wchar_t *buffer, std::size_t length) {
    return std::fgetws(buffer, length, *this);
  }
  template <std::size_t N> auto get(wchar_t (&buffer)[N]) {
    return std::fgetws(buffer, N, *this);
  }
  FILE &put(const wchar_t *buffer) {
    std::fputws(buffer, *this);
    return *this;
  }
  auto unget(wint_t c) { return std::ungetwc(c, *this); }

  auto tell() { return std::ftell(*this); }
  template <int origin> FILE &seek(long offset) {
    static_assert(origin == SEEK_SET || origin == SEEK_CUR || origin = SEEK_END,
                  "Provided seek origin invalid, choose one of SEEK_SET, "
                  "SEEK_CUR or SEEK_END");
    std::fseek(*this, offset, origin);
    return *this;
  }

  auto getpos() {
    std::fpos_t pos;
    std::fgetpos(*this, &pos);
    return pos;
  }
  FILE &setpos(const std::fpos_t &pos) {
    std::fsetpos(*this, &pos);
    return *this;
  }

  FILE &rewind() {
    std::rewind(*this);
    return *this;
  }

  FILE &clearerr() {
    std::clearerr(*this);
    return *this;
  }
  bool eof() { return std::feof(*this); }
  bool error() { return std::ferror(*this); }

  static FILE tmpfile() {
    auto f = std::tmpfile();
    return FILE::steal(f);
  }

  // The formatted input/output functions retain the use of variadic arguments
  // for simplicity. May include in the future versions which solely use type
  // deduction on variadic templates in lieu of format strings.
  template <typename... T> auto scan(const char *format, T *... t) {
    return std::fscanf(*this, format, t...);
  }

  template <typename... T> auto scan(const wchar_t *format, T *... t) {
    return std::fwscanf(*this, format, t...);
  }

  template <typename... T> auto scan(const char *format, va_list vlist) {
    return std::vfscanf(*this, format, vlist);
  }

  template <typename... T> auto scan(const wchar_t *format, va_list vlist) {
    return std::vfwscanf(*this, format, vlist);
  }

  template <typename... T> auto print(const char *format, T &&... t) {
    return std::fprintf(*this, format, t...);
  }

  template <typename... T> auto print(const wchar_t *format, T &&... t) {
    return std::fwprintf(*this, format, t...);
  }

  template <typename... T> auto print(const char *format, va_list vlist) {
    return std::vfprintf(*this, format, vlist);
  }

  template <typename... T> auto print(const wchar_t *format, va_list vlist) {
    return std::vfwprintf(*this, format, vlist);
  }
};
}
}

#endif
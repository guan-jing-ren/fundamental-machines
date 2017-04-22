#ifndef _BASIC_PLUGIN_H_
#define _BASIC_PLUGIN_H_

#include <dlfcn.h>
#include <memory>
#include <string>
#include <vector>

template <typename S> class basic_symbol {
  std::string name;
  std::weak_ptr<void> symbol;

public:
  basic_symbol() = default;
  basic_symbol(const basic_symbol &) = default;
  basic_symbol(basic_symbol &&) = default;
  basic_symbol &operator=(const basic_symbol &) = default;
  basic_symbol &operator=(basic_symbol &&) = default;
  basic_symbol(std::string name, std::shared_ptr<void> sym)
      : name(name), symbol(sym) {
    if (!sym)
      throw std::runtime_error{"Symbol not found"};
  }
  explicit operator bool() { return !!symbol.lock(); }

  template <typename SS = S,
            typename = std::enable_if_t<!std::is_pointer<SS>::value>>
  explicit operator SS() {
    auto locked = std::static_pointer_cast<S>(symbol.lock());
    if (!locked)
      throw std::runtime_error{"DLL unloaded"};
    return *locked;
  }

  template <typename... Args>
  auto operator()(Args &&... args) -> decltype(std::declval<S>()(args...)) {
    auto locked = symbol.lock();
    if (!locked)
      throw std::runtime_error{"DLL unloaded"};
    return (*reinterpret_cast<S *>(locked.get()))(args...);
  }
};

class basic_plugin {
  std::unique_ptr<void, int (*)(void *)> dll;
  std::vector<std::shared_ptr<void>> symbols;

  static void no_op_deleter(void *) {}

public:
  basic_plugin(const char *name)
      : dll(dlopen(name, RTLD_LOCAL | RTLD_NOW), dlclose) {
    if (!dll)
      throw std::runtime_error{"DLL not found"};
  }

  template <typename S> basic_symbol<S> sym(const char *symbol) {
    auto shared_sym =
        std::shared_ptr<void>{dlsym(dll.get(), symbol), no_op_deleter};
    if (shared_sym)
      symbols.push_back(shared_sym);
    return {symbol, shared_sym};
  }
};

#endif
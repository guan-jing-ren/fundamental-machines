#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

using namespace std;

template <typename T, typename... U>
class History : public enable_shared_from_this<History<T, U...>> {
  friend int main();
  struct private_constructor_tag {};
  variant<shared_ptr<const History>, shared_ptr<const T>> base;
  optional<variant<pair<U T::*, U>...>> delta;

public:
  History(shared_ptr<const History> h, private_constructor_tag) : base(h) {}

  template <typename... V>
  History(private_constructor_tag, V &&... v)
      : base(make_shared<T>(forward<V>(v)...)) {}

  ~History() {
    auto *dustbin = get_if<shared_ptr<const History>>(&base);
    while (dustbin && dustbin->unique()) {
      auto previous = move(*dustbin);
      base = move(previous->base);
      dustbin = get_if<shared_ptr<const History>>(&base);
    }
  }

  template <typename... V> static auto create(V &&... v) {
    return make_shared<const History>(private_constructor_tag{},
                                      forward<V>(v)...);
  };

  template <typename V, typename W> auto modify(V T::*f, W &&w) const {
    auto history = make_shared<History>(this->shared_from_this(),
                                        private_constructor_tag{});
    history->delta = make_pair(f, forward<W>(w));
    return history;
  }

  template <typename V> const V &at(V T::*f) const {
    const shared_ptr<const T> *principle = nullptr;
    const History *past = this;
    auto *field = delta ? get_if<pair<V T::*, V>>(&*delta) : nullptr;

    while (!principle && (!field || field->first != f)) {
      field = nullptr;
      principle = get_if<shared_ptr<const T>>(&past->base);
      auto *past_ptr = get_if<shared_ptr<const History>>(&past->base);
      if (past_ptr) {
        past = past_ptr->get();
        field = past->delta ? get_if<pair<V T::*, V>>(&*past->delta) : nullptr;
      }
    }

    if (principle)
      return principle->get()->*f;

    return field->second;
  }

  auto back() const {
    auto *past = get_if<shared_ptr<const History>>(&base);
    if (!past)
      return decltype(*past){};
    return *past;
  }

  auto initial() const {
    auto *principle = get_if<shared_ptr<const T>>(&base);
    if (!principle)
      return decltype(*principle){};
    return *principle;
  }
};

struct Object {
  int field = 42;
  string name = "Object 1";
  long _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, A, B, C, D, E, F;
};

int main() {
  auto object = History<Object, int, string, long>::create();
  cout << "T0   object: " << object->at(&Object::field) << ' '
       << object->at(&Object::name) << '\n';
  auto old = object;

  object =
      object->modify(&Object::field, 98)->modify(&Object::name, "Object 2");

  {
    auto branch = object;
    for (auto i = 0; i < 10'000'000; ++i)
      branch = branch->modify(&Object::F, i);

    // while (branch) {
    //   cout << "Branch::F: " << branch->at(&Object::F) << '\n';
    //   branch = branch->back();
    // }
  }

  cout << "T-0  object: " << object->at(&Object::field) << ' '
       << object->at(&Object::name) << '\n';
  cout << "T-1  object: " << object->back()->at(&Object::field) << ' '
       << object->back()->at(&Object::name) << '\n';
  cout << "T-2  object: " << object->back()->back()->at(&Object::field) << ' '
       << object->back()->back()->at(&Object::name) << '\n';
  cout << "T0   object: " << old->at(&Object::field) << ' '
       << old->at(&Object::name) << '\n';
  cout << "Is old object == T-2 object ? " << boolalpha
       << (old == object->back()->back()) << noboolalpha << '\n';
  cout << object->back()->back()->back().get() << '\n';
  cout << old.get() << " vs " << object->back()->back()->initial().get()
       << '\n';

  return 0;
}

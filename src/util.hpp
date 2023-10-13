#pragma once

#include <concepts>
#include <memory>

template <class Derived, class Base>
requires std::derived_from<Derived, Base>
inline bool instanceof(const Base *ptr) {
    return dynamic_cast<Derived*>(ptr) != nullptr;
}

template <class Derived, class Base>
requires std::derived_from<Derived, Base>
inline bool instanceof(const std::shared_ptr<Base> &ptr) {
    return dynamic_cast<Derived*>(ptr.get()) != nullptr;
}

template <class Derived, class Base>
requires std::derived_from<Derived, Base>
inline std::shared_ptr<Derived> downcast(const std::shared_ptr<Base> &ptr) {
    return std::dynamic_pointer_cast<Derived>(ptr);
}
#include "type.hpp"

using namespace clpl;

std::string NamedType::toString() const {
    return name.identName;
}

std::string IndexedPointerType::toString() const {
    return dataType->toString() + "[]";
}

std::string ReferencePointerType::toString() const {
    return dataType->toString() + "*";
}
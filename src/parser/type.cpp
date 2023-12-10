#include "type.hpp"

using namespace clpl;

bool operator ==(const Type &lhs, const Type &rhs) {
    return lhs.toString() == rhs.toString();
}

std::string NamedType::toString() const {
    return name.identName;
}

bool NamedType::isSigned() const {
    auto n = toString();

    return n == "i8" || n == "i16" || n == "i32" || n == "i64";
}

std::string IndexedPointerType::toString() const {
    return dataType->toString() + "[]";
}

std::string ReferencePointerType::toString() const {
    return dataType->toString() + "*";
}

std::string FunctionReferenceType::toString() const {
    std::string out = "func(";
    for (auto &i : argTypes) {
        out += i->toString() + ",";
    }
    out.pop_back();
    out += "->" + returnType->toString() + ")";
    return out;
}
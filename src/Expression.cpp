#include "Expression.hpp"

Expression::Expression(Type t) : type(t) {}

Expression::~Expression() {
    for (size_t i = 0; i < children.size(); ++i)
        delete children[i];
}

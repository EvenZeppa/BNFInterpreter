#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

struct Expression {
    enum Type {
        EXPR_SEQUENCE,
        EXPR_ALTERNATIVE,
        EXPR_OPTIONAL,
        EXPR_REPEAT,
        EXPR_SYMBOL,
        EXPR_TERMINAL
    };

    Type type;
    std::vector<Expression*> children;
    std::string value;

    Expression(Type t);
    ~Expression();
};

#endif

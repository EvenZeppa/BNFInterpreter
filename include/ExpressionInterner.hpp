#ifndef EXPRESSION_INTERNER_HPP
#define EXPRESSION_INTERNER_HPP

#include <map>
#include <string>
#include <vector>
#include "Expression.hpp"

struct ExpressionKey {
    int type;
    std::string value;
    int rangeStart;
    int rangeEnd;
    std::string bitmapBits;
    std::vector<size_t> childrenIds;

    ExpressionKey();
    explicit ExpressionKey(const Expression* expr);
    bool operator<(const ExpressionKey& other) const;
};

class ExpressionInterner {
public:
    ExpressionInterner();
    Expression* intern(Expression* expr, bool allocatedWithArena);

private:
    std::map<ExpressionKey, Expression*> table;
};

#endif // EXPRESSION_INTERNER_HPP

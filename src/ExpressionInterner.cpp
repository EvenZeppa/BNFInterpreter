#include "../include/ExpressionInterner.hpp"

ExpressionKey::ExpressionKey() : type(0), rangeStart(0), rangeEnd(0) {}

static std::string bitmapToString(const std::bitset<256>& bits) {
    std::string out;
    out.reserve(256);
    for (size_t i = 0; i < 256; ++i) {
        out.push_back(bits.test(i) ? '1' : '0');
    }
    return out;
}

ExpressionKey::ExpressionKey(const Expression* expr)
    : type(static_cast<int>(expr->type)),
      value(expr->value),
      rangeStart(static_cast<int>(expr->charRange.start)),
      rangeEnd(static_cast<int>(expr->charRange.end)),
      bitmapBits(bitmapToString(expr->charBitmap)) {
    for (size_t i = 0; i < expr->children.size(); ++i) {
        childrenIds.push_back(reinterpret_cast<size_t>(expr->children[i]));
    }
}

bool ExpressionKey::operator<(const ExpressionKey& other) const {
    if (type != other.type) return type < other.type;
    if (value != other.value) return value < other.value;
    if (rangeStart != other.rangeStart) return rangeStart < other.rangeStart;
    if (rangeEnd != other.rangeEnd) return rangeEnd < other.rangeEnd;
    if (bitmapBits != other.bitmapBits) return bitmapBits < other.bitmapBits;
    if (childrenIds.size() != other.childrenIds.size()) return childrenIds.size() < other.childrenIds.size();
    for (size_t i = 0; i < childrenIds.size(); ++i) {
        if (childrenIds[i] != other.childrenIds[i]) return childrenIds[i] < other.childrenIds[i];
    }
    return false;
}

ExpressionInterner::ExpressionInterner() {}

Expression* ExpressionInterner::intern(Expression* expr, bool allocatedWithArena) {
    ExpressionKey key(expr);
    std::map<ExpressionKey, Expression*>::iterator it = table.find(key);
    if (it != table.end()) {
        if (!allocatedWithArena) delete expr;
        return it->second;
    }
    table.insert(std::make_pair(key, expr));
    return expr;
}

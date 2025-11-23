#include "BNFParser.hpp"
#include "Expression.hpp"
#include <iostream>
#include <cstring>

BNFParser::BNFParser(const Grammar& g)
    : grammar(g)
{
}

BNFParser::~BNFParser() {}

std::string BNFParser::stripQuotes(const std::string& s) const{
    if (s.size() >= 2 && ((s[0] == '\'' && s[s.size()-1] == '\'') ||
                          (s[0] == '"'  && s[s.size()-1] == '"')))
    {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

ASTNode* BNFParser::parse(const std::string& ruleName,
                          const std::string& input,
                          size_t& consumed) const
{
    consumed = 0;

    Rule* r = grammar.getRule(ruleName);
    if (!r) {
        std::cerr << "BNFParser::parse: rule not found: " << ruleName << std::endl;
        return 0;
    }

    size_t pos = 0;
    ASTNode* root = 0;
    bool ok = parseExpression(r->rootExpr, input, pos, root);

    if (!ok) {
        if (root) delete root;
        return 0;
    }

    consumed = pos;   // <--- important : on exporte ce que le parser a mangé

    return root;
}


bool BNFParser::parseExpression(Expression* expr,
                                const std::string& input,
                                size_t& pos,
                                ASTNode*& outNode) const
{
    if (!expr) return false;

    switch (expr->type) {
        case Expression::EXPR_TERMINAL: {
            std::string literal = stripQuotes(expr->value);
            size_t len = literal.size();
            if (len == 0) return false;

            if (pos + len <= input.size() && input.compare(pos, len, literal) == 0) {
                ASTNode* node = new ASTNode(literal);
                node->matched = literal;
                pos += len;
                outNode = node;
                return true;
            }
            return false;
        }

        case Expression::EXPR_SYMBOL: {
            Rule* rr = grammar.getRule(expr->value);
            if (!rr) {
                std::cerr << "BNFParser::parseExpression: unknown symbol " << expr->value << std::endl;
                return false;
            }
            size_t savedPos = pos;
            ASTNode* child = 0;
            bool ok = parseExpression(rr->rootExpr, input, pos, child);
            if (!ok) {
                pos = savedPos;
                return false;
            }
            ASTNode* node = new ASTNode(expr->value);
            if (child) {
                node->children.push_back(child);
                node->matched = child->matched;
            }
            outNode = node;
            return true;
        }

        case Expression::EXPR_SEQUENCE: {
            size_t savedPos = pos;
            std::vector<ASTNode*> tmpChildren;
            std::string matchedAccum;

            for (size_t i = 0; i < expr->children.size(); ++i) {
                ASTNode* childNode = 0;
                bool ok = parseExpression(expr->children[i], input, pos, childNode);
                if (!ok) {
                    for (size_t j = 0; j < tmpChildren.size(); ++j)
                        delete tmpChildren[j];
                    pos = savedPos;
                    return false;
                }
                tmpChildren.push_back(childNode);
                if (childNode) matchedAccum += childNode->matched;
            }

            ASTNode* parent = new ASTNode("<seq>");
            parent->matched = matchedAccum;
            parent->children.reserve(tmpChildren.size());
            for (size_t k = 0; k < tmpChildren.size(); ++k)
                parent->children.push_back(tmpChildren[k]);

            outNode = parent;
            return true;
        }

        case Expression::EXPR_ALTERNATIVE: {
            ASTNode* bestNode = 0;
            size_t bestPos = pos;
            bool anyMatch = false;

            for (size_t i = 0; i < expr->children.size(); ++i) {
                size_t savedPos = pos;
                ASTNode* branchNode = 0;
                bool ok = parseExpression(expr->children[i], input, pos, branchNode);

                if (ok) {
                    anyMatch = true;
                    if (pos > bestPos) {
                        if (bestNode) delete bestNode;
                        bestNode = new ASTNode("<alt>");
                        bestNode->children.push_back(branchNode);
                        bestNode->matched = branchNode->matched;
                        bestPos = pos;
                    } else {
                        delete branchNode;
                    }
                }
                pos = savedPos;
            }

            if (!anyMatch) return false;
            pos = bestPos;
            outNode = bestNode;
            return true;
        }

        case Expression::EXPR_OPTIONAL: {
            size_t savedPos = pos;
            ASTNode* inside = 0;
            bool ok = parseExpression(expr->children[0], input, pos, inside);
            if (!ok) {
                pos = savedPos;
                ASTNode* node = new ASTNode("<opt>");
                node->matched = "";
                outNode = node;
                return true;
            }
            ASTNode* node = new ASTNode("<opt>");
            if (inside) {
                node->children.push_back(inside);
                node->matched = inside->matched;
            }
            outNode = node;
            return true;
        }

        case Expression::EXPR_REPEAT: {
            std::vector<ASTNode*> items;
            std::string matchedAccum;
            while (true) {
                size_t iterSaved = pos;
                ASTNode* it = 0;
                bool ok = parseExpression(expr->children[0], input, pos, it);
                if (!ok) {
                    pos = iterSaved;
                    break;
                }
                if (it && it->matched.empty()) {
                    delete it;
                    pos = iterSaved;
                    break;
                }
                if (it) {
                    matchedAccum += it->matched;
                    items.push_back(it);
                } else {
                    break;
                }
                if (pos >= input.size()) break;
            }
            ASTNode* parent = new ASTNode("<rep>");
            parent->matched = matchedAccum;
            for (size_t i = 0; i < items.size(); ++i)
                parent->children.push_back(items[i]);
            outNode = parent;
            return true;
        }

        default:
            std::cerr << "BNFParser::parseExpression: unsupported expr type\n";
            return false;
    }
}

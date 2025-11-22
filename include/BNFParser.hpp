#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include "Expression.hpp"
#include "Grammar.hpp"

class ParseResult
{
public:
    bool success;
    Expression* node;
    size_t nextPos;

    ParseResult(bool s = false, Expression* n = 0, size_t p = 0)
        : success(s), node(n), nextPos(p) {}
};

class BNFParser
{
public:
    BNFParser(const Grammar& grammar);
    ~BNFParser();

    // Parse depuis la règle 'ruleName'
    ParseResult parse(const std::string& input, const std::string& ruleName);

private:
    const Grammar& grammar;

    // fonctions de parsing internes
    ParseResult parseExpression(Expression* expr, const std::string& input, size_t pos);
    ParseResult parseNonTerminal(Expression* expr, const std::string& input, size_t pos);
    ParseResult parseTerminal(Expression* expr, const std::string& input, size_t pos);
    ParseResult parseSequence(Expression* expr, const std::string& input, size_t pos);
    ParseResult parseChoice(Expression* expr, const std::string& input, size_t pos);
    ParseResult parseZeroOrMore(Expression* expr, const std::string& input, size_t pos);
};

#endif

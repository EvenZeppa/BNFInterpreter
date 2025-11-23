// src/BNFParser.hpp
#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include "Grammar.hpp"
#include "AST.hpp"
#include <string>

class BNFParser {
public:
    BNFParser(const Grammar& g);
    ~BNFParser();

	ASTNode* parse(const std::string& ruleName,
				const std::string& input,
				size_t& consumed) const;

private:
    const Grammar& grammar;

    std::string stripQuotes(const std::string& s) const;

    // fonction interne récursive pour construire AST
    bool parseExpression(Expression* expr,
                         const std::string& input,
                         size_t& pos,
                         ASTNode*& outNode) const;
};

#endif

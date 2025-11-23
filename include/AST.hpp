#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <iostream>

struct ASTNode {
    std::string symbol;
    std::string matched;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& s);
    ~ASTNode();
};

void printAST(const ASTNode* node, int indent = 0);

#endif

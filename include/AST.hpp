#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>

struct ASTNode {
    std::string symbol;
    std::string matched;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& s);
    ~ASTNode();
};

#endif

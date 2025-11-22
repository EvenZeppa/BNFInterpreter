#include "AST.hpp"

ASTNode::ASTNode(const std::string& s) : symbol(s) {}
ASTNode::~ASTNode() {
    for (size_t i = 0; i < children.size(); ++i)
        delete children[i];
}

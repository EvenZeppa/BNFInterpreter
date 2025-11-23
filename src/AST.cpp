#include "AST.hpp"

ASTNode::ASTNode(const std::string& s) : symbol(s) {}

ASTNode::~ASTNode() {
    for (size_t i = 0; i < children.size(); ++i)
        delete children[i];
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  "; // deux espaces par niveau
}

void printAST(const ASTNode* node, int indent) {
    if (!node) {
        printIndent(indent);
        std::cout << "(null)\n";
        return;
    }

    printIndent(indent);

    // Affichage du nœud
    std::cout << node->symbol;

    // matched utile pour comprendre les répétitions et alternatives
    if (!node->matched.empty())
        std::cout << "  [matched=\"" << node->matched << "\"]";

    std::cout << "\n";

    // Affichage récursif des enfants
    for (size_t i = 0; i < node->children.size(); ++i)
        printAST(node->children[i], indent + 1);
}

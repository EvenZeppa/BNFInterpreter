#include "UnitTest.hpp"
#include "AST.hpp"
#include <sstream>
#include <iostream>

void test_node_creation() {
    ASTNode* node = new ASTNode("root");
    ASSERT_EQ(node->symbol, "root");
    ASSERT_TRUE(node->matched.empty());
    ASSERT_EQ(node->children.size(), 0);
    delete node;
}

void test_node_with_match() {
    ASTNode* node = new ASTNode("letter");
    node->matched = "A";
    ASSERT_EQ(node->symbol, "letter");
    ASSERT_EQ(node->matched, "A");
    delete node;
}

void test_add_children() {
    ASTNode* root = new ASTNode("root");
    ASTNode* child1 = new ASTNode("child1");
    ASTNode* child2 = new ASTNode("child2");

    root->children.push_back(child1);
    root->children.push_back(child2);

    ASSERT_EQ(root->children.size(), 2);
    ASSERT_EQ(root->children[0]->symbol, "child1");
    ASSERT_EQ(root->children[1]->symbol, "child2");

    delete root; // doit aussi delete les enfants
}

void test_nested_tree() {
    ASTNode* root = new ASTNode("root");
    ASTNode* branch = new ASTNode("branch");
    ASTNode* leaf = new ASTNode("leaf");

    branch->children.push_back(leaf);
    root->children.push_back(branch);

    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->children.size(), 1);
    ASSERT_EQ(root->children[0]->children[0]->symbol, "leaf");

    delete root; // doit delete tous les enfants
}

// Vérification de l'affichage récursif (juste pour s'assurer que printAST ne crash pas)
void test_printAST() {
    ASTNode* root = new ASTNode("root");
    ASTNode* child = new ASTNode("child");
    child->matched = "X";
    root->children.push_back(child);

    std::ostringstream oss;
    std::streambuf* oldCout = std::cout.rdbuf(oss.rdbuf());
    printAST(root);
    std::cout.rdbuf(oldCout);

    std::string output = oss.str();
    ASSERT_TRUE(output.find("root") != std::string::npos);
    ASSERT_TRUE(output.find("child") != std::string::npos);
    ASSERT_TRUE(output.find("X") != std::string::npos);

    delete root;
}

int main() {
    std::cout << "Running test_node_creation...\n"; test_node_creation();
    std::cout << "Running test_node_with_match...\n"; test_node_with_match();
    std::cout << "Running test_add_children...\n"; test_add_children();
    std::cout << "Running test_nested_tree...\n"; test_nested_tree();
    std::cout << "Running test_printAST...\n"; test_printAST();

    printTestSummary();
    return failed == 0 ? 0 : 1;
}

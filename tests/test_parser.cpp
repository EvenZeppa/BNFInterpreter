#include "UnitTest.hpp"
#include "Grammar.hpp"
#include "BNFParser.hpp"
#include "Expression.hpp"
#include <iostream>
#include <string>

//
//  Utilitaire simple: compte les noeuds dans l'AST
//
int countAST(ASTNode* n) {
    if (!n) return 0;
    int c = 1;
    for (size_t i = 0; i < n->children.size(); ++i)
        c += countAST(n->children[i]);
    return c;
}

//
//  TEST 1 : parsing terminal simple
//
void test_parse_terminal() {
    std::cout << "Running test_parse_terminal...\n";

    Grammar g;
    g.addRule("<A> ::= 'HELLO'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<A>", "HELLO", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "HELLO");
    ASSERT_EQ(consumed, 5);

    // Un AST terminal = 1 noeud
    ASSERT_EQ(countAST(ast), 1);

    delete ast;
}

//
//  TEST 2 : parsing terminal FAIL
//
void test_parse_terminal_fail() {
    std::cout << "Running test_parse_terminal_fail...\n";

    Grammar g;
    g.addRule("<A> ::= 'HELLO'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<A>", "HALLO", consumed);

    ASSERT_TRUE(ast == 0);
    ASSERT_EQ(consumed, 0);
}

//
//  TEST 3 : séquence simple
//
void test_parse_sequence() {
    std::cout << "Running test_parse_sequence...\n";

    Grammar g;
    g.addRule("<seq> ::= 'A' 'B' 'C'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<seq>", "ABC", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "ABC");
    ASSERT_EQ(consumed, 3);

    ASSERT_EQ(ast->children.size(), 3);

    delete ast;
}

//
//  TEST 4 : alternative (+ longest match)
//
void test_parse_alternative() {
    std::cout << "Running test_parse_alternative...\n";

    Grammar g;
    g.addRule("<alt> ::= 'A' | 'AB' | 'ABC'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<alt>", "ABC", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "ABC");
    ASSERT_EQ(consumed, 3);

    delete ast;
}

//
//  TEST 5 : alternative FAIL
//
void test_parse_alternative_fail() {
    std::cout << "Running test_parse_alternative_fail...\n";

    Grammar g;
    g.addRule("<alt> ::= 'A' | 'B'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<alt>", "C", consumed);

    ASSERT_TRUE(ast == 0);
    ASSERT_EQ(consumed, 0);
}

//
//  TEST 6 : optional
//
void test_parse_optional() {
    std::cout << "Running test_parse_optional...\n";

    Grammar g;
    g.addRule("<opt> ::= 'A' [ 'B' ] 'C'");
    BNFParser p(g);

    size_t consumed = 0;

    // Cas 1 : optionnel présent
    ASTNode* ast1 = p.parse("<opt>", "ABC", consumed);
    ASSERT_TRUE(ast1 != 0);
    ASSERT_EQ(ast1->matched, "ABC");
    ASSERT_EQ(consumed, 3);
    delete ast1;

    // Cas 2 : optionnel absent
    consumed = 0;
    ASTNode* ast2 = p.parse("<opt>", "AC", consumed);
    ASSERT_TRUE(ast2 != 0);
    ASSERT_EQ(ast2->matched, "AC");
    ASSERT_EQ(consumed, 2);
    delete ast2;

    // Cas 3 : optionnel échoue -> parse doit échouer
    consumed = 0;
    ASTNode* ast3 = p.parse("<opt>", "AXC", consumed);
    ASSERT_TRUE(ast3 == 0);
    ASSERT_EQ(consumed, 0);
}

//
//  TEST 7 : répétition
//
void test_parse_repetition() {
    std::cout << "Running test_parse_repetition...\n";

    Grammar g;
    g.addRule("<rep> ::= 'A' { 'B' }");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<rep>", "ABBB", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "ABBB");
    ASSERT_EQ(consumed, 4);

    ASSERT_EQ(countAST(ast), 1 + 1 + 1 + 3); // seq + A + rep + 3*B

    delete ast;
}

//
//  TEST 8 : symbol references
//
void test_parse_symbol() {
    std::cout << "Running test_parse_symbol...\n";

    Grammar g;
    g.addRule("<digit> ::= '0' | '1'");
    g.addRule("<bin> ::= <digit> <digit> <digit>");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<bin>", "101", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "101");
    ASSERT_EQ(consumed, 3);

    delete ast;
}

//
//  TEST 9 : consommation totale NON obligatoire
//
void test_parse_must_consume_all() {
    std::cout << "Running test_parse_must_consume_all...\n";

    Grammar g;
    g.addRule("<A> ::= 'HI'");
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<A>", "HI!", consumed);

    ASSERT_TRUE(ast != 0);
    ASSERT_EQ(ast->matched, "HI");
    ASSERT_EQ(consumed, 2); // NE DOIT PAS échouer maintenant

    delete ast;
}

//
//  TEST 10 : règle inconnue
//
void test_unknown_rule() {
    std::cout << "Running test_unknown_rule...\n";

    Grammar g; // aucune règle définie
    BNFParser p(g);

    size_t consumed = 0;
    ASTNode* ast = p.parse("<unknown>", "hello", consumed);

    ASSERT_TRUE(ast == 0);
    ASSERT_EQ(consumed, 0);
}

//
//  MAIN
//
int main() {
    test_parse_terminal();
    test_parse_terminal_fail();
    test_parse_sequence();
    test_parse_alternative();
    test_parse_alternative_fail();
    test_parse_optional();
    test_parse_repetition();
    test_parse_symbol();
    test_parse_must_consume_all();
    test_unknown_rule();

    printTestSummary();
    return (failed == 0) ? 0 : 1;
}

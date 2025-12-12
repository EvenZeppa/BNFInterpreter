#include "../include/TestFramework.hpp"
#include "../include/Grammar.hpp"
#include "../include/BNFTokenizer.hpp"
#include "../include/Expression.hpp"
#include <string>
#include <vector>
#include <iostream>

// Fonction utilitaire pour compter le nombre de noeuds dans l'AST
int countNodes(Expression* expr) {
	if (!expr) return 0;
	int cnt = 1;
	for (size_t i = 0; i < expr->children.size(); ++i)
		cnt += countNodes(expr->children[i]);
	return cnt;
}

/**
 * @brief Test simple letter rule parsing.
 */
void test_simple_letter_rule(TestRunner& runner) {
	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B' | 'C'");
	Rule* r = g.getRule("<letter>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);

	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_ALTERNATIVE);
	ASSERT_EQ(runner, expr->children.size(), 3); // A, B, C

	for (size_t i = 0; i < expr->children.size(); ++i)
		ASSERT_EQ(runner, expr->children[i]->type, Expression::EXPR_TERMINAL);
}

/**
 * @brief Test simple nick rule parsing with repetition.
 */
void test_simple_nick_rule(TestRunner& runner) {
	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B' | 'C'");
	g.addRule("<number> ::= '0' | '1' | '2'");
	g.addRule("<nick> ::= <letter> { <letter> | <number> }");

	Rule* r = g.getRule("<nick>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);

	Expression* expr = r->rootExpr;

	// L'AST devrait être une SEQUENCE
	ASSERT_EQ(runner, expr->type, Expression::EXPR_SEQUENCE);
	ASSERT_EQ(runner, expr->children.size(), 2);

	// Premier enfant : <letter>
	ASSERT_EQ(runner, expr->children[0]->type, Expression::EXPR_SYMBOL);
	ASSERT_EQ(runner, expr->children[0]->value, "<letter>");

	// Deuxième enfant : EXPR_REPEAT contenant EXPR_ALTERNATIVE
	Expression* rep = expr->children[1];
	ASSERT_EQ(runner, rep->type, Expression::EXPR_REPEAT);
	ASSERT_EQ(runner, rep->children.size(), 1);

	Expression* alt = rep->children[0];
	ASSERT_EQ(runner, alt->type, Expression::EXPR_ALTERNATIVE);
	ASSERT_EQ(runner, alt->children.size(), 2);

	// Vérifier les enfants de l'alternative
	ASSERT_EQ(runner, alt->children[0]->type, Expression::EXPR_SYMBOL);
	ASSERT_EQ(runner, alt->children[0]->value, "<letter>");
	ASSERT_EQ(runner, alt->children[1]->type, Expression::EXPR_SYMBOL);
	ASSERT_EQ(runner, alt->children[1]->value, "<number>");
}

/**
 * @brief Test simple command rule parsing with complex alternatives.
 */
void test_simple_command_rule(TestRunner& runner) {
	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B' | 'C'");
	g.addRule("<number> ::= '0' | '1' | '2'");
	g.addRule("<command>  ::= <letter> { <letter> } | <number> <number> <number>");

	Rule* r = g.getRule("<command>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);

	Expression* expr = r->rootExpr;
	// L'AST devrait être une ALTERNATIVE
	ASSERT_EQ(runner, expr->type, Expression::EXPR_ALTERNATIVE);
	ASSERT_EQ(runner, expr->children.size(), 2);

	// Premier enfant : SEQUENCE <letter> { <letter> }
	Expression* seq1 = expr->children[0];
	ASSERT_EQ(runner, seq1->type, Expression::EXPR_SEQUENCE);
	ASSERT_EQ(runner, seq1->children.size(), 2);
	ASSERT_EQ(runner, seq1->children[0]->type, Expression::EXPR_SYMBOL);
	ASSERT_EQ(runner, seq1->children[0]->value, "<letter>");
	ASSERT_EQ(runner, seq1->children[1]->type, Expression::EXPR_REPEAT);
	ASSERT_EQ(runner, seq1->children[1]->children.size(), 1);
	ASSERT_EQ(runner, seq1->children[1]->children[0]->type, Expression::EXPR_SYMBOL);
	ASSERT_EQ(runner, seq1->children[1]->children[0]->value, "<letter>");

	// Deuxième enfant : SEQUENCE <number> <number> <number>
	Expression* seq2 = expr->children[1];
	ASSERT_EQ(runner, seq2->type, Expression::EXPR_SEQUENCE);
	ASSERT_EQ(runner, seq2->children.size(), 3);
	for (size_t i = 0; i < 3; ++i) {
		ASSERT_EQ(runner, seq2->children[i]->type, Expression::EXPR_SYMBOL);
		ASSERT_EQ(runner, seq2->children[i]->value, "<number>");
	}
}

/**
 * @brief Test character range parsing.
 */
void test_char_range(TestRunner& runner) {
	Grammar g;
	g.addRule("<lower> ::= 'a' ... 'z'");
	
	Rule* r = g.getRule("<lower>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);
	
	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_CHAR_RANGE);
	ASSERT_EQ(runner, static_cast<int>(expr->charRange.start), static_cast<int>('a'));
	ASSERT_EQ(runner, static_cast<int>(expr->charRange.end), static_cast<int>('z'));
}

/**
 * @brief Test hex range parsing.
 */
void test_hex_range(TestRunner& runner) {
	Grammar g;
	g.addRule("<ascii> ::= 0x00 ... 0x7F");
	
	Rule* r = g.getRule("<ascii>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);
	
	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_CHAR_RANGE);
	ASSERT_EQ(runner, static_cast<int>(expr->charRange.start), 0x00);
	ASSERT_EQ(runner, static_cast<int>(expr->charRange.end), 0x7F);
}

/**
 * @brief Test inclusive character class parsing.
 */
void test_inclusive_char_class(TestRunner& runner) {
	Grammar g;
	g.addRule("<ident> ::= ( 'a' ... 'z' 'A' ... 'Z' '_' )");
	
	Rule* r = g.getRule("<ident>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);
	
	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_CHAR_CLASS);
	ASSERT_EQ(runner, expr->isExclusion, false);
	ASSERT_EQ(runner, expr->rangeList.size(), 2); // 'a'..'z' and 'A'..'Z'
	ASSERT_EQ(runner, expr->charList.size(), 1); // '_'
	
	ASSERT_EQ(runner, static_cast<int>(expr->rangeList[0].start), static_cast<int>('a'));
	ASSERT_EQ(runner, static_cast<int>(expr->rangeList[0].end), static_cast<int>('z'));
	ASSERT_EQ(runner, static_cast<int>(expr->rangeList[1].start), static_cast<int>('A'));
	ASSERT_EQ(runner, static_cast<int>(expr->rangeList[1].end), static_cast<int>('Z'));
	ASSERT_EQ(runner, static_cast<int>(expr->charList[0]), static_cast<int>('_'));
}

/**
 * @brief Test exclusive character class parsing.
 */
void test_exclusive_char_class(TestRunner& runner) {
	Grammar g;
	g.addRule("<nonspace> ::= ( ^ ' ' 0x0A 0x0D )");
	
	Rule* r = g.getRule("<nonspace>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);
	
	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_CHAR_CLASS);
	ASSERT_EQ(runner, expr->isExclusion, true);
	ASSERT_EQ(runner, expr->rangeList.size(), 0);
	ASSERT_EQ(runner, expr->charList.size(), 3); // ' ', 0x0A, 0x0D
	
	ASSERT_EQ(runner, static_cast<int>(expr->charList[0]), static_cast<int>(' '));
	ASSERT_EQ(runner, static_cast<int>(expr->charList[1]), 0x0A);
	ASSERT_EQ(runner, static_cast<int>(expr->charList[2]), 0x0D);
}

/**
 * @brief Test mixed character class with ranges and literals.
 */
void test_mixed_char_class(TestRunner& runner) {
	Grammar g;
	g.addRule("<token> ::= ( '0' ... '9' 'a' ... 'f' 'A' ... 'F' )");
	
	Rule* r = g.getRule("<token>");
	ASSERT_NOT_NULL(runner, r);
	ASSERT_NOT_NULL(runner, r->rootExpr);
	
	Expression* expr = r->rootExpr;
	ASSERT_EQ(runner, expr->type, Expression::EXPR_CHAR_CLASS);
	ASSERT_EQ(runner, expr->isExclusion, false);
	ASSERT_EQ(runner, expr->rangeList.size(), 3); // 0-9, a-f, A-F
	ASSERT_EQ(runner, expr->charList.size(), 0);
}

int main() {
	TestSuite suite("Grammar Test Suite");
	
	// Register all test functions
	suite.addTest("Simple Letter Rule", test_simple_letter_rule);
	suite.addTest("Simple Nick Rule", test_simple_nick_rule);
	suite.addTest("Simple Command Rule", test_simple_command_rule);
	suite.addTest("Character Range", test_char_range);
	suite.addTest("Hex Range", test_hex_range);
	suite.addTest("Inclusive Character Class", test_inclusive_char_class);
	suite.addTest("Exclusive Character Class", test_exclusive_char_class);
	suite.addTest("Mixed Character Class", test_mixed_char_class);
	
	// Run all tests
	TestRunner results = suite.run();
	results.printSummary();
	
	return results.allPassed() ? 0 : 1;
}

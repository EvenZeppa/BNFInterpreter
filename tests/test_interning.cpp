#include "../include/TestFramework.hpp"
#include "../include/Grammar.hpp"
#include "../include/ExpressionInterner.hpp"

void test_interning_shared_alternatives(TestRunner& runner) {
    ExpressionInterner inter;
    Grammar g;
    g.setInterner(&inter);

    g.addRule("<a> ::= 'X' | 'Y'");
    g.addRule("<b> ::= 'X' | 'Y'");

    Rule* ra = g.getRule("<a>");
    Rule* rb = g.getRule("<b>");
    ASSERT_NOT_NULL(runner, ra);
    ASSERT_NOT_NULL(runner, rb);

    ASSERT_EQ(runner, ra->rootExpr, rb->rootExpr);
    ASSERT_EQ(runner, ra->rootExpr->children[0], rb->rootExpr->children[0]);
    ASSERT_EQ(runner, ra->rootExpr->children[1], rb->rootExpr->children[1]);
}

void test_interning_distinct_shapes(TestRunner& runner) {
    ExpressionInterner inter;
    Grammar g;
    g.setInterner(&inter);

    g.addRule("<a> ::= 'X' | 'Y'");
    g.addRule("<b> ::= 'X' 'Y'");

    Rule* ra = g.getRule("<a>");
    Rule* rb = g.getRule("<b>");
    ASSERT_NOT_NULL(runner, ra);
    ASSERT_NOT_NULL(runner, rb);

    ASSERT_NE(runner, ra->rootExpr, rb->rootExpr);
}

int main() {
    TestSuite suite("Interning Test Suite");
    suite.addTest("Shared Alternatives", test_interning_shared_alternatives);
    suite.addTest("Distinct Shapes", test_interning_distinct_shapes);
    TestRunner results = suite.run();
    results.printSummary();
    return results.allPassed() ? 0 : 1;
}

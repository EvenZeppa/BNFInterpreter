#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "Grammar.hpp"
#include "BNFParser.hpp"

// Simple assertion helpers used by all phases
static void expectMatch(const std::string& title,
                        BNFParser& parser,
                        const std::string& rule,
                        const std::string& input,
                        const std::string& expectedMatched) {
    size_t consumed = 0;
    ASTNode* ast = parser.parse(rule, input, consumed);
    assert(ast && "Parser returned null AST");
    assert(consumed == expectedMatched.size());
    assert(ast->matched == expectedMatched);
    std::cout << "  [ok] " << title << " => '" << ast->matched << "'" << std::endl;
    delete ast;
}

static void expectFail(const std::string& title,
                       BNFParser& parser,
                       const std::string& rule,
                       const std::string& input) {
    size_t consumed = 0;
    ASTNode* ast = parser.parse(rule, input, consumed);
    assert(ast == 0);
    std::cout << "  [fail as expected] " << title << " (consumed=" << consumed << ")" << std::endl;
}

// Phase 1: character ranges and inclusive/exclusive character classes
static void phaseRangesAndClasses() {
    std::cout << "\n=== Phase 1: Ranges and Classes ===" << std::endl;
    Grammar g;

    // Character ranges using ellipsis and hex literals
    g.addRule("<lower> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<ascii> ::= 0x00 ... 0x7F");

    // Inclusive and exclusive classes
    g.addRule("<vowel> ::= ( 'a' 'e' 'i' 'o' 'u' )");
    g.addRule("<consonant> ::= ( ^ 'a' 'e' 'i' 'o' 'u' )");
    g.addRule("<token> ::= <lower> <digit>");

    BNFParser parser(g);

    expectMatch("lowercase range", parser, "<lower>", "m", "m");
    expectMatch("digit range", parser, "<digit>", "5", "5");
    expectMatch("ascii full range", parser, "<ascii>", std::string(1, '\x7F'), std::string(1, '\x7F'));
    expectMatch("inclusive class (vowel)", parser, "<vowel>", "i", "i");
    expectMatch("exclusive class (consonant)", parser, "<consonant>", "b", "b");
    expectFail("exclusive class rejects vowel", parser, "<consonant>", "a");
    expectMatch("range sequencing", parser, "<token>", "a7", "a7");

    std::cout << "Phase 1 complete and testable." << std::endl;
}

int main() {
    std::cout << "BNFParserLib Feature Showcase" << std::endl;
    std::cout << "==============================" << std::endl;

    phaseRangesAndClasses();

    return 0;
}

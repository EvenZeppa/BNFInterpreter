#include <iostream>
#include "Grammar.hpp"
#include "BNFParser.hpp"

int main() {
    std::cout << "BNF Parser prototype (C++98)" << std::endl;

    Grammar grammar;
	grammar.addRule("<digit> ::= '0' | '1' | '2'");
	grammar.addRule("<number> ::= <digit> { <digit> }");
	grammar.addRule("<message> ::= ':' <prefix> <SPACE> <command> <params>");

    BNFParser parser(&grammar);

    ASTNode* result = parser.parse("<digit>", "1");

    if (result == 0)
        std::cout << "Parsing not implemented yet." << std::endl;

    return 0;
}

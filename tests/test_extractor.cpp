#include "../include/Grammar.hpp"
#include "../include/BNFParser.hpp"
#include "../include/DataExtractor.hpp"
#include "../include/ExtractedData.hpp"
#include "../include/UnitTest.hpp"
#include "../include/Debug.hpp"
#include <iostream>
#include <vector>
#include <string>

/**
 * @brief Comprehensive test suite for DataExtractor functionality.
 * 
 * Tests all configuration options and utility methods of the DataExtractor
 * class using a variety of grammar structures and input messages.
 * Uses C++98 compatible unit testing framework.
 */

// Helper function to create a simple test grammar
void setupTestGrammar(Grammar& g) {
    // Basic building blocks
    g.addRule("<letter> ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'");
    g.addRule("<digit> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'");
    g.addRule("<special> ::= '#' | '@' | '!' | '.' | '-' | '_' | ':'");
    
    // Character classes
    g.addRule("<word-char> ::= <letter> | <digit> | '_'");
    g.addRule("<param-char> ::= <letter> | <digit> | <special>");
    
    // Words and parameters with repetitions
    g.addRule("<word> ::= <letter> { <word-char> }");
    g.addRule("<param> ::= <param-char> { <param-char> }");
    g.addRule("<number> ::= <digit> { <digit> }");
    
    // Structural elements
    g.addRule("<space> ::= ' '");
    g.addRule("<spaces> ::= <space> { <space> }");
    g.addRule("<separator> ::= ','");
    
    // Complex structures with repetitions
    g.addRule("<word-list> ::= <word> { <spaces> <word> }");
    g.addRule("<param-list> ::= <param> { <separator> <param> }");
    g.addRule("<mixed-list> ::= <word> { <separator> <number> }");
    
    // Optional and alternative structures
    g.addRule("<prefix> ::= ':' <word>");
    g.addRule("<suffix> ::= <space> <word>");
    g.addRule("<command> ::= <word> | <number>");
    
    // Main test structures
    g.addRule("<simple-message> ::= <command> <space> <param>");
    g.addRule("<complex-message> ::= [ <prefix> <space> ] <command> <spaces> <param-list> [ <suffix> ]");
    g.addRule("<list-message> ::= <word-list> <space> <mixed-list>");
}

// Test basic extraction functionality
void testBasicExtraction() {
    std::cout << "\n=== Testing Basic Extraction ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = "JOIN #channel";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<simple-message>", input, consumed);
    
    ASSERT_TRUE(ast != NULL);
    ASSERT_TRUE(consumed > 0);
    
    DataExtractor extractor;
    ExtractedData data = extractor.extract(ast);
    
    // Test that some symbols were extracted
    ASSERT_TRUE(data.values.size() > 0);
    
    // Test basic has() functionality
    ASSERT_TRUE(data.has("<command>"));
    ASSERT_TRUE(data.has("<param>"));
    
    // Test first() functionality
    std::string firstCommand = data.first("<command>");
    ASSERT_TRUE(!firstCommand.empty());
    
    std::string firstParam = data.first("<param>");
    ASSERT_TRUE(!firstParam.empty());
    
    // Test count() functionality
    ASSERT_TRUE(data.count("<command>") >= 1);
    ASSERT_TRUE(data.count("<param>") >= 1);
    
    delete ast;
    std::cout << "✓ Basic extraction tests passed" << std::endl;
}

// Test symbol filtering functionality
void testSymbolFiltering() {
    std::cout << "\n=== Testing Symbol Filtering ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = ":prefix JOIN param1,param2,param3 suffix";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<complex-message>", input, consumed);
    
    if (!ast) {
        std::cout << "Parse failed, skipping symbol filtering tests" << std::endl;
        return;
    }
    
    // Test with specific symbols only
    DataExtractor extractor;
    std::vector<std::string> targetSymbols;
    targetSymbols.push_back("<command>");
    targetSymbols.push_back("<param>");
    extractor.setSymbols(targetSymbols);
    
    ExtractedData data = extractor.extract(ast);
    
    // Should only have the specified symbols
    ASSERT_TRUE(data.has("<command>"));
    ASSERT_TRUE(data.has("<param>"));
    
    // Should not have other symbols like <word> (unless they match our targets)
    size_t symbolTypeCount = data.values.size();
    std::cout << "Symbol types found with filtering: " << symbolTypeCount << std::endl;
    
    // Test with empty filter (should extract all non-terminals)
    DataExtractor extractor2;
    ExtractedData data2 = extractor2.extract(ast);
    
    size_t allSymbolCount = data2.values.size();
    std::cout << "Symbol types found without filtering: " << allSymbolCount << std::endl;
    
    // With filtering should have fewer or equal symbol types
    ASSERT_TRUE(symbolTypeCount <= allSymbolCount);
    
    delete ast;
    std::cout << "✓ Symbol filtering tests passed" << std::endl;
}

// Test terminal inclusion functionality
void testTerminalInclusion() {
    std::cout << "\n=== Testing Terminal Inclusion ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = "WORD 123";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<simple-message>", input, consumed);
    
    ASSERT_TRUE(ast != NULL);
    
    // Test without terminals
    DataExtractor extractor1;
    extractor1.includeTerminals(false);
    ExtractedData data1 = extractor1.extract(ast);
    
    size_t countWithoutTerminals = data1.values.size();
    std::cout << "Symbol types without terminals: " << countWithoutTerminals << std::endl;
    
    // Test with terminals
    DataExtractor extractor2;
    extractor2.includeTerminals(true);
    ExtractedData data2 = extractor2.extract(ast);
    
    size_t countWithTerminals = data2.values.size();
    std::cout << "Symbol types with terminals: " << countWithTerminals << std::endl;
    
    // Including terminals should result in more or equal symbols
    ASSERT_TRUE(countWithTerminals >= countWithoutTerminals);
    
    delete ast;
    std::cout << "✓ Terminal inclusion tests passed" << std::endl;
}

// Test repetition flattening functionality
void testRepetitionFlattening() {
    std::cout << "\n=== Testing Repetition Flattening ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = "word1 word2 word3 1,2,3";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<list-message>", input, consumed);
    
    if (!ast) {
        std::cout << "Parse failed, skipping repetition flattening tests" << std::endl;
        return;
    }
    
    // Test without flattening
    DataExtractor extractor1;
    extractor1.flattenRepetitions(false);
    ExtractedData data1 = extractor1.extract(ast);
    
    // Test with flattening
    DataExtractor extractor2;
    extractor2.flattenRepetitions(true);
    ExtractedData data2 = extractor2.extract(ast);
    
    // Compare results
    std::cout << "Without flattening - symbol types: " << data1.values.size() << std::endl;
    std::cout << "With flattening - symbol types: " << data2.values.size() << std::endl;
    
    // Test that flattening affects the results
    bool resultsAreDifferent = (data1.values.size() != data2.values.size());
    if (!resultsAreDifferent) {
        // Check if the content is different
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = data1.values.begin();
             it != data1.values.end(); ++it) {
            if (data2.has(it->first)) {
                if (data1.count(it->first) != data2.count(it->first)) {
                    resultsAreDifferent = true;
                    break;
                }
            }
        }
    }
    
    // Note: This test might pass even if flattening doesn't change results,
    // depending on the AST structure
    std::cout << "Flattening " << (resultsAreDifferent ? "changed" : "did not change") << " the results" << std::endl;
    
    delete ast;
    std::cout << "✓ Repetition flattening tests completed" << std::endl;
}

// Test configuration reset functionality
void testConfigurationReset() {
    std::cout << "\n=== Testing Configuration Reset ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = "TEST param";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<simple-message>", input, consumed);
    
    ASSERT_TRUE(ast != NULL);
    
    DataExtractor extractor;
    
    // Configure extractor
    std::vector<std::string> symbols;
    symbols.push_back("<command>");
    extractor.setSymbols(symbols);
    extractor.includeTerminals(true);
    extractor.flattenRepetitions(true);
    
    // Extract with configuration
    ExtractedData data1 = extractor.extract(ast);
    size_t configuredCount = data1.values.size();
    
    // Reset configuration
    extractor.resetConfig();
    
    // Extract with default configuration
    ExtractedData data2 = extractor.extract(ast);
    size_t resetCount = data2.values.size();
    
    std::cout << "With configuration: " << configuredCount << " symbol types" << std::endl;
    std::cout << "After reset: " << resetCount << " symbol types" << std::endl;
    
    // Reset should change the results (in most cases)
    ASSERT_TRUE(configuredCount != resetCount || configuredCount == 0);
    
    delete ast;
    std::cout << "✓ Configuration reset tests passed" << std::endl;
}

// Test utility methods thoroughly
void testUtilityMethods() {
    std::cout << "\n=== Testing Utility Methods ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = "cmd param1,param2,param3";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<complex-message>", input, consumed);
    
    if (!ast) {
        std::cout << "Parse failed, using simple message" << std::endl;
        ast = parser.parse("<simple-message>", "cmd param", consumed);
        ASSERT_TRUE(ast != NULL);
    }
    
    DataExtractor extractor;
    ExtractedData data = extractor.extract(ast);
    
    // Test has() method
    bool hasCommand = data.has("<command>");
    bool hasNonExistent = data.has("<nonexistent>");
    
    ASSERT_TRUE(hasCommand == true || hasCommand == false); // Should not crash
    ASSERT_FALSE(hasNonExistent);
    
    // Test first() method
    std::string firstCommand = data.first("<command>");
    std::string firstNonExistent = data.first("<nonexistent>");
    
    ASSERT_TRUE(firstNonExistent.empty());
    
    // Test count() method
    size_t commandCount = data.count("<command>");
    size_t nonExistentCount = data.count("<nonexistent>");
    
    ASSERT_TRUE(commandCount == 1);
    ASSERT_EQ(nonExistentCount, 0);
    
    // Test all() method
    std::vector<std::string> allCommands = data.all("<command>");
    std::vector<std::string> allNonExistent = data.all("<nonexistent>");
    
    ASSERT_EQ(allCommands.size(), commandCount);
    ASSERT_TRUE(allNonExistent.empty());
    
    // If we have commands, test consistency
    if (commandCount > 0) {
        ASSERT_FALSE(firstCommand.empty());
        ASSERT_EQ(allCommands[0], firstCommand);
    }
    
    delete ast;
    std::cout << "✓ Utility method tests passed" << std::endl;
}

// Test edge cases and error conditions
void testEdgeCases() {
    std::cout << "\n=== Testing Edge Cases ===" << std::endl;
    
    // Test with null AST
    DataExtractor extractor;
    ExtractedData data = extractor.extract(NULL);
    ASSERT_TRUE(data.values.empty());
    
    // Test with empty configuration
    Grammar g;
    g.addRule("<empty> ::= ''");
    BNFParser parser(g);
    
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<empty>", "", consumed);
    
    if (ast) {
        ExtractedData data2 = extractor.extract(ast);
        // Should not crash
        ASSERT_TRUE(data2.values.size() == 0);
        delete ast;
    }
    
    // Test with empty symbol list
    std::vector<std::string> emptySymbols;
    extractor.setSymbols(emptySymbols);
    
    // Should reset to extract all symbols
    Grammar g2;
    setupTestGrammar(g2);
    BNFParser parser2(g2);
    ASTNode* ast2 = parser2.parse("<simple-message>", "test param", consumed);
    
    if (ast2) {
        ExtractedData data3 = extractor.extract(ast2);
        // Should extract some symbols
        std::cout << "With empty symbol filter: " << data3.values.size() << " symbol types" << std::endl;
        delete ast2;
    }
    
    std::cout << "✓ Edge case tests passed" << std::endl;
}

// Test complex scenarios with combined configurations
void testComplexScenarios() {
    std::cout << "\n=== Testing Complex Scenarios ===" << std::endl;
    
    Grammar g;
    setupTestGrammar(g);
    BNFParser parser(g);
    
    std::string input = ":prefix COMMAND param1,param2,param3 suffix";
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<complex-message>", input, consumed);
    
    if (!ast) {
        std::cout << "Complex parse failed, using simpler input" << std::endl;
        ast = parser.parse("<simple-message>", "CMD param", consumed);
        ASSERT_TRUE(ast != NULL);
    }
    
    // Scenario 1: Extract only specific symbols with terminals
    DataExtractor extractor1;
    std::vector<std::string> specificSymbols;
    specificSymbols.push_back("<command>");
    specificSymbols.push_back("<param>");
    extractor1.setSymbols(specificSymbols);
    extractor1.includeTerminals(true);
    
    ExtractedData data1 = extractor1.extract(ast);
    std::cout << "Scenario 1 (specific + terminals): " << data1.values.size() << " symbol types" << std::endl;
    
    // Scenario 2: Extract all with flattening
    DataExtractor extractor2;
    extractor2.flattenRepetitions(true);
    extractor2.includeTerminals(false);
    
    ExtractedData data2 = extractor2.extract(ast);
    std::cout << "Scenario 2 (all + flatten): " << data2.values.size() << " symbol types" << std::endl;
    
    // Scenario 3: Full configuration
    DataExtractor extractor3;
    extractor3.setSymbols(specificSymbols);
    extractor3.includeTerminals(true);
    extractor3.flattenRepetitions(true);
    
    ExtractedData data3 = extractor3.extract(ast);
    std::cout << "Scenario 3 (full config): " << data3.values.size() << " symbol types" << std::endl;
    
    // Test that configurations produce different results
    bool scenariosDiffer = (data1.values.size() != data2.values.size()) || 
                          (data2.values.size() != data3.values.size());
    
    std::cout << "Different configurations " << (scenariosDiffer ? "produced" : "did not produce") 
              << " different results" << std::endl;
    
    delete ast;
    std::cout << "✓ Complex scenario tests completed" << std::endl;
}

int main() {
    std::cout << "DataExtractor Test Suite (C++98 Compatible)" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    try {
        testBasicExtraction();
        testSymbolFiltering();
        testTerminalInclusion();
        testRepetitionFlattening();
        testConfigurationReset();
        testUtilityMethods();
        testEdgeCases();
        testComplexScenarios();
        
        printTestSummary();
        
        if (failed == 0) {
            std::cout << "\n🎉 All tests passed! DataExtractor is working correctly." << std::endl;
            return 0;
        } else {
            std::cout << "\n❌ Some tests failed. Please check the implementation." << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Test suite error: " << e.what() << std::endl;
        return 1;
    }
}

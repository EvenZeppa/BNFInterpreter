#include "../include/TestFramework.hpp"
#include "../include/Grammar.hpp"
#include "../include/BNFParser.hpp"
#include <string>

/**
 * @brief Integration test: IRC-style nickname with character ranges and classes.
 * 
 * Tests a complete IRC nickname parser using:
 * - Character ranges for letters and numbers
 * - Character classes for valid identifier characters
 * - Repetition and sequences
 */
void test_irc_nickname_with_ranges(TestRunner& runner) {
    Grammar g;
    
    // Define character classes using new syntax
    g.addRule("<letter> ::= ( 'a' ... 'z' 'A' ... 'Z' )");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<special> ::= ( '-' '[' ']' '{' '}' '\\' '`' '^' '_' '|' )");
    g.addRule("<nick-char> ::= <letter> | <digit> | <special>");
    g.addRule("<nickname> ::= <letter> { <nick-char> }");
    
    BNFParser p(g);
    size_t consumed = 0;
    
    // Valid nicknames
    ASTNode* ast = p.parse("<nickname>", "Alice", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "Alice");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<nickname>", "Bob123", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "Bob123");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<nickname>", "user_name", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "user_name");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<nickname>", "test[bot]", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "test[bot]");
    delete ast;
    
    // Invalid nicknames (start with digit)
    consumed = 0;
    ast = p.parse("<nickname>", "123user", consumed);
    ASSERT_TRUE(runner, ast == 0);
}

/**
 * @brief Integration test: Hexadecimal number parser.
 */
void test_hex_number_parser(TestRunner& runner) {
    Grammar g;
    
    g.addRule("<hex-digit> ::= ( '0' ... '9' 'a' ... 'f' 'A' ... 'F' )");
    g.addRule("<hex-number> ::= '0' 'x' <hex-digit> { <hex-digit> }");
    
    BNFParser p(g);
    size_t consumed = 0;
    
    // Valid hex numbers
    ASTNode* ast = p.parse("<hex-number>", "0xFF", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "0xFF");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<hex-number>", "0x1234ABCD", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "0x1234ABCD");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<hex-number>", "0x0", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "0x0");
    delete ast;
    
    // Invalid hex numbers
    consumed = 0;
    ast = p.parse("<hex-number>", "0xGHI", consumed);
    ASSERT_TRUE(runner, ast == 0);
    
    consumed = 0;
    ast = p.parse("<hex-number>", "xFF", consumed);  // Missing '0x'
    ASSERT_TRUE(runner, ast == 0);
}

/**
 * @brief Integration test: String with excluded characters.
 * 
 * Tests parsing strings that exclude certain characters (e.g., no whitespace or special chars).
 */
void test_non_whitespace_string(TestRunner& runner) {
    Grammar g;
    
    // Character class excluding whitespace and control characters
    g.addRule("<printable> ::= ( ^ ' ' 0x09 0x0A 0x0D )");
    g.addRule("<word> ::= <printable> { <printable> }");
    
    BNFParser p(g);
    size_t consumed = 0;
    
    // Valid words (no whitespace)
    ASTNode* ast = p.parse("<word>", "hello", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "hello");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<word>", "test-123", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "test-123");
    delete ast;
    
    // Should stop at whitespace
    consumed = 0;
    ast = p.parse("<word>", "hello world", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "hello");
    ASSERT_EQ(runner, consumed, 5);  // Only consumed "hello"
    delete ast;
}

/**
 * @brief Integration test: Email-like identifier.
 */
void test_email_identifier(TestRunner& runner) {
    Grammar g;
    
    g.addRule("<alphanum> ::= ( 'a' ... 'z' 'A' ... 'Z' '0' ... '9' )");
    g.addRule("<local> ::= <alphanum> { <alphanum> | '.' | '_' | '-' }");
    g.addRule("<domain-part> ::= <alphanum> { <alphanum> | '-' }");
    g.addRule("<domain> ::= <domain-part> { '.' <domain-part> }");
    g.addRule("<email> ::= <local> '@' <domain>");
    
    BNFParser p(g);
    size_t consumed = 0;
    
    // Valid email-like identifiers
    ASTNode* ast = p.parse("<email>", "user@example.com", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "user@example.com");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<email>", "test.user@sub.domain.org", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "test.user@sub.domain.org");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<email>", "user_name@host-name.net", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "user_name@host-name.net");
    delete ast;
    
    // Invalid (missing @)
    consumed = 0;
    ast = p.parse("<email>", "userexample.com", consumed);
    ASSERT_TRUE(runner, ast == 0);
}

/**
 * @brief Integration test: Mixed ranges and character classes in alternatives.
 */
void test_complex_token_parser(TestRunner& runner) {
    Grammar g;
    
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<letter> ::= ( 'a' ... 'z' 'A' ... 'Z' )");
    g.addRule("<alphanum> ::= ( 'a' ... 'z' 'A' ... 'Z' '0' ... '9' )");
    g.addRule("<number> ::= <digit> { <digit> }");
    g.addRule("<ident-start> ::= <letter> | '_'");
    g.addRule("<ident-char> ::= <alphanum> | '_'");
    g.addRule("<identifier> ::= <ident-start> { <ident-char> }");
    g.addRule("<token> ::= <number> | <identifier>");
    
    BNFParser p(g);
    size_t consumed = 0;
    
    // Numbers
    ASTNode* ast = p.parse("<token>", "42", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "42");
    delete ast;
    
    // Identifiers
    consumed = 0;
    ast = p.parse("<token>", "variable", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "variable");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<token>", "_private", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "_private");
    delete ast;
    
    consumed = 0;
    ast = p.parse("<token>", "var_123", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "var_123");
    delete ast;
}

int main() {
    TestSuite suite("Integration Test Suite: Character Ranges and Classes");
    
    suite.addTest("IRC Nickname with Ranges", test_irc_nickname_with_ranges);
    suite.addTest("Hexadecimal Number Parser", test_hex_number_parser);
    suite.addTest("Non-Whitespace String", test_non_whitespace_string);
    suite.addTest("Email Identifier", test_email_identifier);
    suite.addTest("Complex Token Parser", test_complex_token_parser);
    
    TestRunner results = suite.run();
    results.printSummary();
    
    return results.allPassed() ? 0 : 1;
}

#include "BNFTokenizer.hpp"
#include <cctype>

Token::Token(Type t, const std::string& v)
    : type(t), value(v) {}

BNFTokenizer::BNFTokenizer(const std::string& input)
    : text(input), pos(0) {}

void BNFTokenizer::skipSpaces() {
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t'))
        ++pos;
}

Token BNFTokenizer::peek() {
    size_t save = pos;
    Token t = next();
    pos = save;
    return t;
}

Token BNFTokenizer::next() {
    skipSpaces();

    if (pos >= text.size())
        return Token(Token::TOK_END, "");

    char c = text[pos];

    // Symbol <...>
    if (c == '<')
        return parseSymbol();

    // Terminal '...' or "..."
    if (c == '\'' || c == '"')
        return parseTerminal();

    // Single-character tokens
    if (c == '{') { pos++; return Token(Token::TOK_LBRACE, "{"); }
    if (c == '}') { pos++; return Token(Token::TOK_RBRACE, "}"); }
    if (c == '[') { pos++; return Token(Token::TOK_LBRACKET, "["); }
    if (c == ']') { pos++; return Token(Token::TOK_RBRACKET, "]"); }
    if (c == '|') { pos++; return Token(Token::TOK_PIPE, "|"); }

    // Word (fallback)
    return parseWord();
}

Token BNFTokenizer::parseSymbol() {
    size_t start = pos++;
    while (pos < text.size() && text[pos] != '>')
        pos++;
    if (pos < text.size()) pos++; // include '>'
    return Token(Token::TOK_SYMBOL, text.substr(start, pos - start));
}

Token BNFTokenizer::parseTerminal() {
    char quote = text[pos];
    size_t start = pos++;
    while (pos < text.size() && text[pos] != quote)
        pos++;
    if (pos < text.size()) pos++; // include closing quote
    return Token(Token::TOK_TERMINAL, text.substr(start, pos - start));
}

Token BNFTokenizer::parseWord() {
    size_t start = pos;
    while (pos < text.size() &&
           !isspace(text[pos]) &&
           text[pos] != '|' &&
           text[pos] != '{' && text[pos] != '}' &&
           text[pos] != '[' && text[pos] != ']')
    {
        pos++;
    }
    return Token(Token::TOK_WORD, text.substr(start, pos - start));
}

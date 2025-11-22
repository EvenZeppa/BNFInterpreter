#ifndef BNF_TOKENIZER_HPP
#define BNF_TOKENIZER_HPP

#include <string>
#include <vector>

struct Token {
    enum Type {
        TOK_SYMBOL,
        TOK_TERMINAL,
        TOK_LBRACE,   // {
        TOK_RBRACE,   // }
        TOK_LBRACKET, // [
        TOK_RBRACKET, // ]
        TOK_PIPE,     // |
        TOK_WORD,     // un mot simple
        TOK_END
    };

    Type type;
    std::string value;

    Token(Type t, const std::string& v);
};

class BNFTokenizer {
public:
    BNFTokenizer(const std::string& input);
    Token next();
    Token peek();

private:
    std::string text;
    size_t pos;

    void skipSpaces();
    Token parseSymbol();
    Token parseTerminal();
    Token parseWord();
};

#endif

#include "Grammar.hpp"
#include "BNFTokenizer.hpp"
#include <iostream>

Rule::Rule() : rootExpr(0) {}
Rule::~Rule() { delete rootExpr; }

Grammar::Grammar() {}
Grammar::~Grammar() {
    for (size_t i = 0; i < rules.size(); ++i)
        delete rules[i];
}

void Grammar::addRule(const std::string& ruleText) {
    size_t pos = ruleText.find("::=");
    if (pos == std::string::npos) {
        std::cerr << "Invalid rule: " << ruleText << std::endl;
        return;
    }

    std::string lhs = ruleText.substr(0, pos);
    std::string rhs = ruleText.substr(pos + 3);

    // trim
    // (TODO améliorer)
    while (!lhs.empty() && lhs[0] == ' ') lhs.erase(0, 1);
    while (!lhs.empty() && lhs[lhs.size()-1] == ' ') lhs.erase(lhs.size()-1, 1);

    Rule* r = new Rule();
    r->name = lhs;

    BNFTokenizer tz(rhs);
    r->rootExpr = parseExpression(tz);

    rules.push_back(r);
}


Rule* Grammar::getRule(const std::string& name) {
    for (size_t i = 0; i < rules.size(); ++i)
        if (rules[i]->name == name)
            return rules[i];
    return 0;
}

Expression* Grammar::parseExpression(BNFTokenizer& tz) {
    // expr = seq { '|' seq }
    Expression* left = parseSequence(tz);
    Token t = tz.peek();
    if (t.type != Token::TOK_PIPE)
        return left;

    Expression* alt = new Expression(Expression::EXPR_ALTERNATIVE);
    alt->children.push_back(left);

    while (tz.peek().type == Token::TOK_PIPE) {
        tz.next(); // skip |
        Expression* right = parseSequence(tz);
        alt->children.push_back(right);
    }
    return alt;
}

Expression* Grammar::parseSequence(BNFTokenizer& tz) {
    Expression* seq = new Expression(Expression::EXPR_SEQUENCE);

    while (true) {
        Token t = tz.peek();
        if (t.type == Token::TOK_END ||
            t.type == Token::TOK_PIPE ||
            t.type == Token::TOK_RBRACE ||
            t.type == Token::TOK_RBRACKET)
            break;

        Expression* term = parseTerm(tz);
        if (term)
            seq->children.push_back(term);
    }

    return seq;
}

Expression* Grammar::parseTerm(BNFTokenizer& tz) {
    Token t = tz.peek();

    if (t.type == Token::TOK_LBRACE) {
        tz.next();
        Expression* inside = parseExpression(tz);
        if (tz.next().type != Token::TOK_RBRACE)
            std::cerr << "Missing '}'" << std::endl;

        Expression* rep = new Expression(Expression::EXPR_REPEAT);
        rep->children.push_back(inside);
        return rep;
    }

    if (t.type == Token::TOK_LBRACKET) {
        tz.next();
        Expression* inside = parseExpression(tz);
        if (tz.next().type != Token::TOK_RBRACKET)
            std::cerr << "Missing ']'" << std::endl;

        Expression* opt = new Expression(Expression::EXPR_OPTIONAL);
        opt->children.push_back(inside);
        return opt;
    }

    return parseFactor(tz);
}

Expression* Grammar::parseFactor(BNFTokenizer& tz) {
    Token t = tz.next();

    if (t.type == Token::TOK_SYMBOL) {
        Expression* e = new Expression(Expression::EXPR_SYMBOL);
        e->value = t.value;
        return e;
    }

    if (t.type == Token::TOK_TERMINAL) {
        Expression* e = new Expression(Expression::EXPR_TERMINAL);
        e->value = t.value;
        return e;
    }

    if (t.type == Token::TOK_WORD) {
        Expression* e = new Expression(Expression::EXPR_TERMINAL);
        e->value = t.value;
        return e;
    }

    std::cerr << "Unexpected token: " << t.value << std::endl;
    return 0;
}
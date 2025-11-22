#include "BNFParser.hpp"
#include <iostream>

BNFParser::BNFParser(const Grammar& g)
    : grammar(g)
{
}

BNFParser::~BNFParser()
{
}

ParseResult BNFParser::parse(const std::string& input, const std::string& ruleName)
{
    Rule* r = grammar.getRule(ruleName);
    if (!r)
        return ParseResult(false, 0, 0);

    return parseExpression(r->expression, input, 0);
}

ParseResult BNFParser::parseExpression(Expression* expr, const std::string& input, size_t pos)
{
    switch (expr->type)
    {
        case EXPR_TERMINAL:
            return parseTerminal(expr, input, pos);

        case EXPR_NON_TERMINAL:
            return parseNonTerminal(expr, input, pos);

        case EXPR_SEQUENCE:
            return parseSequence(expr, input, pos);

        case EXPR_CHOICE:
            return parseChoice(expr, input, pos);

        case EXPR_ZERO_OR_MORE:
            return parseZeroOrMore(expr, input, pos);

        default:
            return ParseResult(false, 0, pos);
    }
}

/******** TERMINAL ********/
ParseResult BNFParser::parseTerminal(Expression* expr, const std::string& input, size_t pos)
{
    const std::string& t = static_cast<TerminalExpression*>(expr)->value;

    if (input.compare(pos, t.size(), t) == 0)
    {
        return ParseResult(true, expr, pos + t.size());
    }
    return ParseResult(false, 0, pos);
}

/******** NON-TERMINAL ********/
ParseResult BNFParser::parseNonTerminal(Expression* expr, const std::string& input, size_t pos)
{
    NonTerminalExpression* nt = static_cast<NonTerminalExpression*>(expr);
    Rule* r = const_cast<Grammar&>(grammar).getRule(nt->ruleName);
    if (!r)
        return ParseResult(false, 0, pos);

    return parseExpression(r->expression, input, pos);
}

/******** SEQUENCE ********/
ParseResult BNFParser::parseSequence(Expression* expr, const std::string& input, size_t pos)
{
    SequenceExpression* seq = static_cast<SequenceExpression*>(expr);
    size_t currentPos = pos;

    for (size_t i = 0; i < seq->elements.size(); ++i)
    {
        ParseResult r = parseExpression(seq->elements[i], input, currentPos);
        if (!r.success)
            return ParseResult(false, 0, pos); // rollback complet

        currentPos = r.nextPos;
    }
    return ParseResult(true, expr, currentPos);
}

/******** CHOICE (altération) ********/
ParseResult BNFParser::parseChoice(Expression* expr, const std::string& input, size_t pos)
{
    ChoiceExpression* c = static_cast<ChoiceExpression*>(expr);

    for (size_t i = 0; i < c->choices.size(); ++i)
    {
        ParseResult r = parseExpression(c->choices[i], input, pos);
        if (r.success)
            return r;
    }
    return ParseResult(false, 0, pos);
}

/******** ZERO OR MORE ********/
ParseResult BNFParser::parseZeroOrMore(Expression* expr, const std::string& input, size_t pos)
{
    ZeroOrMoreExpression* z = static_cast<ZeroOrMoreExpression*>(expr);
    size_t currentPos = pos;

    while (1)
    {
        ParseResult r = parseExpression(z->element, input, currentPos);
        if (!r.success)
            break;

        currentPos = r.nextPos;

        if (currentPos >= input.size())
            break;
    }

    return ParseResult(true, expr, currentPos);
}

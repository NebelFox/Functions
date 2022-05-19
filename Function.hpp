#ifndef INC_4_FUNCTIONS_FUNCTION_HPP
#define INC_4_FUNCTIONS_FUNCTION_HPP


#include <iostream>
#include <stack>
#include <string>
#include <set>
#include <map>
#include <list>
#include <cmath>

#include "Token.hpp"

using std::string;
using std::stod;

class Function
{
    friend class Compiler;

private:
    string _infix;
    string _postfix;
    std::list<Unit> _expression;

    explicit Function(const std::list<Unit>& expression,
                      const string& infix,
                      const string& postfix)
    {
        _expression = expression;
        _infix = infix;
        _postfix = postfix;
    }

public:
    using Args = std::map<string, double>;

    [[nodiscard]] const string& postfix() const
    {
        return _postfix;
    }

    [[nodiscard]] const string& infix() const
    {
        return _infix;
    }

    [[nodiscard]] double evaluate(const Args& args) const
    {
        std::stack<double> stack;
        for (const auto& unit: _expression)
            stack.push(unit(stack, args));
        return stack.top();
    }

    [[nodiscard]] double evaluate() const
    {
        Args args;
        return this->evaluate(args);
    }

    double operator()(const Args& args) const
    {
        return evaluate(args);
    }

    double operator()() const
    {
        return evaluate();
    }
};


#endif //INC_4_FUNCTIONS_FUNCTION_HPP

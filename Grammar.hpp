#ifndef INC_4_FUNCTIONS_GRAMMAR_HPP
#define INC_4_FUNCTIONS_GRAMMAR_HPP

#include <string>
#include <map>

using std::string;
using std::map;

class Grammar
{
public:
    using Precedence = unsigned char;

    typedef double (* Unary)(const double);
    typedef double (* Binary)(const double, const double);

private:
    struct BinaryOperator
    {
        Binary binary = nullptr;
        Precedence precedence = 0;
    };

    map<string, double> _constants;
    map<string, Unary> _prefixOperators;
    map<string, BinaryOperator> _binaryOperators;
    map<string, Unary> _postfixOperators;

public:
    [[nodiscard]] const map<string, double>& constants() const
    {
        return _constants;
    }

    [[nodiscard]] const map<string, Unary>& prefix() const
    {
        return _prefixOperators;
    }

    [[nodiscard]] const map<string, BinaryOperator>& binary() const
    {
        return _binaryOperators;
    }

    [[nodiscard]] const map<string, Unary>& postfix() const
    {
        return _postfixOperators;
    }

    static size_t matchNumber(const string& s, const size_t start)
    {
        bool isSigned = (s[start] == '-' || s[start] == '+');
        size_t length = isSigned;
        while (isdigit(s[start + length])) ++length;
        length += s[start + length] == '.';
        while (isdigit(s[start + length])) ++length;
        return length * (!isSigned || length > 1);
    }

    static size_t matchArgument(const string& s, const size_t start)
    {
        size_t length = 0;
        while (isalpha(s[start + length]) || s[start + length] == '_') ++length;
        return length;
    }

    size_t matchPrefix(const string& s, const size_t start)
    {
        return match(s, start, _prefixOperators);
    }

    size_t matchBinary(const string& s, const size_t start)
    {
        return match(s, start, _binaryOperators);
    }

    size_t matchPostfix(const string& s, const size_t start)
    {
        return match(s, start, _postfixOperators);
    }

    Precedence precedence(const string& signature)
    {
        auto lookup = _binaryOperators.find(signature);
        return (lookup != _binaryOperators.end()) ? lookup->second.precedence : 0;
    }

    void addConstant(const string& name, const double value)
    {
        _constants.insert_or_assign(name, value);
    }

    void addPrefixOperator(const string& signature, Unary prefix)
    {
        _prefixOperators.insert_or_assign(signature, prefix);
    }

    void addBinaryOperator(const string& signature, Binary binary, const Precedence precedence)
    {
        _binaryOperators.insert_or_assign(signature, BinaryOperator {binary, precedence});
    }

    void addPostfixOperator(const string& signature, Unary postfix)
    {
        _postfixOperators.insert_or_assign(signature, postfix);
    }

private:
    template<typename Operator>
    static size_t match(const string& s, const size_t start, map<string, Operator>& ops)
    {
        size_t length;
        for (const auto& pair: ops)
        {
            length = pair.first.length();
            if (start + length <= s.length() && s.compare(start, length, pair.first) == 0)
            {
                return length;
            }
        }
        return 0;
    }
};


#endif //INC_4_FUNCTIONS_GRAMMAR_HPP

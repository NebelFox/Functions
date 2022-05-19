#ifndef INC_4_FUNCTIONS_TOKEN_HPP
#define INC_4_FUNCTIONS_TOKEN_HPP

#include <string>
#include <stack>
#include <functional>
#include <map>

using Stack = std::stack<double>&;
using Args = const std::map<std::string, double>&;
using Unit = std::function<double (Stack, Args)>;


enum TokenType : unsigned char
{
    TT_NUMBER   = 0b00000001,
    TT_PREFIX   = 0b00000010,
    TT_BINARY   = 0b00000100,
    TT_POSTFIX  = 0b00001000,
    TT_OPEN     = 0b00010000,
    TT_CLOSE    = 0b00100000,
    TT_ARGUMENT = 0b01000000
};


struct Token
{
    TokenType type;
    std::string value;
};


#endif //INC_4_FUNCTIONS_TOKEN_HPP

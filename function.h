//
// Created by NebelFox on 09.05.2021.
//

#ifndef INC_4_FUNCTIONS_FUNCTION_H
#define INC_4_FUNCTIONS_FUNCTION_H

#include <iostream>

#include <string>
#include <set>
#include <map>
#include <list>
#include <cmath>
#include <exception>

#include <bitset>

#ifdef USE_HANDMADE_STACK
#include "stash.hpp"
#else
#include "stack.hpp"
#endif

using std::string;
using std::stod;

class Function
{
public:
    using precedence_t = unsigned char;
    typedef double (*unary_operation_t) (const double);
    typedef double (*binary_operation_t) (const double, const double);

private:
    enum UnitType : unsigned char {
        UT_NONE               = 0b00000000,
        UT_NUMBER             = 0b00000001,
        UT_PREFIX_FUNCTION    = 0b00000010,
        UT_BINARY_OPERATOR    = 0b00000100,
        UT_POSTFIX_FUNCTION   = 0b00001000,
        UT_OPENING_BRACKET    = 0b00010000,
        UT_CLOSING_BRACKET    = 0b00100000,
        UT_ARGUMENT           = 0b01000000
    };
    struct Unit {
        UnitType type;
        string value;
    };

    struct BinaryOperation {
        precedence_t precedence=0;
        binary_operation_t function = nullptr;
    };

    static std::map<string, double> constants;
    static std::map<string, unary_operation_t> prefixFunctions;
    static std::map<string, BinaryOperation> binaryOperations;
    static std::map<string, unary_operation_t> postfixFunctions;

    string _infix;
    std::list<Unit> _expression;

public:
    static bool overrideRegisters;
    static bool enableConstants;

    struct Exception : public std::exception
    {
        [[maybe_unused]] string info;
        [[maybe_unused]] string file;
        [[maybe_unused]] string scope;
        [[maybe_unused]] size_t line;
        Exception (
            string && info_,
            string && file_,
            string && scope_,
            size_t line_
        );

    };


    static bool registerConstant (string && signature, double value);
    static bool registerPrefixFunction (string && signature, unary_operation_t function);
    static bool registerBinaryOperation (string && signature, binary_operation_t function, precedence_t precedence);
    static bool registerPostfixFunction (string && signature, unary_operation_t function);

    static const std::map<string, double>& allConstants ();
    static const std::map<string, unary_operation_t>& allPrefix ();
    static const std::map<string, BinaryOperation>& allBinary ();
    static const std::map<string, unary_operation_t>& allPostfix ();

    static void basic_init ();

    static size_t matchNumber (const string & instance, size_t start);
    static size_t matchArgument (const string & instance, size_t start);
    static size_t matchPrefixFunction (const string & instance, size_t start);
    static size_t matchBinaryOperator (const string & instance, size_t start);
    static size_t matchPostfixFunction (const string & instance, size_t start);

    static precedence_t precedence_of (const string & signature);

    using args_t = std::map<string, double>;

    Function () = default;
    explicit Function (string infix);
    Function (const Function & instance);

    string asString () const;
    const string& infix () const;

    double evaluate (const args_t & args) const;
    double evaluate () const;
    double operator() (const args_t & args) const;
    double operator () () const;
};


#endif //INC_4_FUNCTIONS_FUNCTION_H

#ifndef INC_4_FUNCTIONS_CALCULATOR_HPP
#define INC_4_FUNCTIONS_CALCULATOR_HPP


#include <regex>
#include <functional>

#include "Grammar.hpp"
#include "Compiler.hpp"
#include "Function.hpp"


using std::map;
using std::string;
using std::cin;
using std::cout;
using std::endl;

using std::regex;
using std::sregex_iterator;
using std::smatch;
using std::regex_match;


class Calculator
{
private:
    Grammar _grammar;
    Compiler* _compiler;
    map<string, Function> _functions;
    map<string, std::function<void()>> _commands;
    const regex _argsPattern;

    static void setupGrammar(Grammar& grammar)
    {
        grammar.addConstant("pi", M_PI);
        grammar.addConstant("e", M_E);

        grammar.addPrefixOperator("-",
                                  [](const double x) -> double
                                  { return -x; });
        grammar.addPrefixOperator("exp",
                                  [](const double x) -> double
                                  { return std::exp(x); });
        grammar.addPrefixOperator("sin",
                                  [](const double x) -> double
                                  { return std::sin(x); });
        grammar.addPrefixOperator("cos",
                                  [](const double x) -> double
                                  { return std::cos(x); });
        grammar.addPrefixOperator("floor",
                                  [](const double x) -> double
                                  { return std::floor(x); });
        grammar.addPrefixOperator("ceil",
                                  [](const double x) -> double
                                  { return std::ceil(x); });
        grammar.addPrefixOperator("round",
                                  [](const double x) -> double
                                  { return std::round(x); });

        grammar.addBinaryOperator("+",
                                  [](const double a, const double b) -> double
                                  { return a + b; }, 1);
        grammar.addBinaryOperator("-",
                                  [](const double a, const double b) -> double
                                  { return a - b; }, 1);
        grammar.addBinaryOperator("*",
                                  [](const double a, const double b) -> double
                                  { return a * b; }, 2);
        grammar.addBinaryOperator("/",
                                  [](const double a, const double b) -> double
                                  { return a / b; }, 2);
        grammar.addBinaryOperator("^", pow, 3);

        grammar.addPostfixOperator("!", [](const double x) -> double
        {
            auto n = (size_t) x;
            size_t result = 1;
            while (n > 1)
            {
                result *= n--;
            }
            return (double) result;
        });
    }

public:
    Calculator() : _argsPattern("(?:([a-zA-Z_]+)=([\\-0-9.]+)*)")
    {
        setupGrammar(_grammar);
        _compiler = new Compiler(_grammar);
        _commands["save"] = [&](){save();};
        _commands["eval"] = [&](){ eval();};
        _commands["evals"] = [&](){ evalSaved();};
        _commands["show"] = [&](){show();};
        _commands["list-saved"] = [&](){listSaved();};
        _commands["delete"] = [&](){deleteSaved();};
        _commands["clear"] = [&](){clear();};
        _commands["grammar"] = [&](){grammar();};
        _commands["args-info"] = argsInfo;
        _commands["help"] = help;

    }

    void dialogue()
    {
        bool running = true;
        string keyword;
        help();
        cout << endl;
        while (running)
        {
            cout << "$~";
            cin >> keyword;
            auto lookup = _commands.find(keyword);
            if(lookup != _commands.end())
                lookup->second();
            else if(keyword == "exit")
                running = false;
            else
                cout << "Unknown command" << endl;
            cout << endl;
        }
    }

    void save()
    {
        string name;
        cin >> name >> std::ws;
        string expression;
        getline(cin, expression);
//        cout << "save | name=[" << name << "] expression=[" << expression << "]\n";
        _functions.insert_or_assign(name, _compiler->compile(expression));
    }

    void eval()
    {
        string tail;
        getline(cin >> std::ws, tail);
        auto iterator = sregex_iterator(tail.begin(),
                                        tail.end(),
                                        _argsPattern);
        auto end = sregex_iterator();
        size_t argsBegin = string::npos;
        if(iterator != end)
            argsBegin = (*iterator).position(0);
        Function::Args args;
        for(; iterator != end; ++iterator)
        {
            args[(*iterator)[1].str()] = stod((*iterator)[2].str());
        }
        string expression = tail.substr(0, argsBegin);
//        cout << "Expression: [" << expression << "]\n";
        Function f = _compiler->compile(expression);
        double result = f(args);
        cout << "The result = " << result << endl;
    }

    void evalSaved()
    {
        string name;
        cin >> name >> std::ws;
        Function::Args args;
        string tail;
        getline(cin, tail);
        auto iterator = sregex_iterator(tail.begin(),
                                        tail.end(),
                                        _argsPattern);
        auto end = sregex_iterator();
        for (; iterator != end; ++iterator)
            args.insert_or_assign((*iterator)[1].str(), stod((*iterator)[2].str()));
        auto lookup = _functions.find(name);
        if (lookup != _functions.end())
            cout << "The result is: " << lookup->second(args) << endl;
        else
            cout << "Unknown function: '" << name << "'\n";
    }

    void show()
    {
        string name;
        cin >> name;
        auto lookup = _functions.find(name);
        if (lookup != _functions.end())
        {
            cout << "Infix form: "
                 << lookup->second.infix()
                 << "\nPostfix form: "
                 << lookup->second.postfix()
                 << endl;
        }
    }

    void listSaved()
    {
        if (_functions.empty())
        {
            cout << "No functions\n";
        }
        else
        {
            for (const auto& pair: _functions)
                cout << pair.first << ": " << pair.second.infix() << endl;
        }
    }

    void deleteSaved()
    {
        string name;
        cin >> name;
        _functions.erase(name);
    }

    void clear()
    {
        _functions.clear();
    }

    void grammar()
    {
        cout << "Constants: ";
        for (const auto& pair: _grammar.constants())
            cout << pair.first << '=' << pair.second << ", ";
        cout << "\nPrefix Functions: ";
        for (const auto& pair: _grammar.prefix())
            cout << pair.first << ", ";
        cout << "\nBinary Operations (precedence): ";
        for (const auto& pair: _grammar.binary())
            cout << pair.first << '(' << (short) pair.second.precedence << "), ";
        cout << "\nPostfix Functions: ";
        for (const auto& pair: _grammar.postfix())
            cout << pair.first << ", ";
        cout << endl;
    }

    static void argsInfo()
    {
        static const string message(
                "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
                "# 'args...' is a sequence of 'name=value' pairs, where:   #\n"
                "# > name - argument signature to seek in function for     #\n"
                "# > value - int or float number to substitute 'name' with #\n"
                "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
                "# Pairs should be separated with at least one whitespace  #\n"
                "# Order of pairs does not matter                          #\n"
                "# Extra pairs are ignored                                 #\n"
                "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
        );
        cout << message;
    }

    static void help()
    {
        static const string message(
                "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
                "# AVAILABLE COMMANDS:                                           #\n"
                "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
                "# > eval function args...   - eval expression with given args   #\n"
                "# > save name function      - save the function as 'name'       #\n"
                "# > evals name args...      - eval saved function 'name'        #\n"
                "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
                "# > show name   - 'name' function in infix & postfix notations  #\n"
                "# > list-saved  - all saved functions                           #\n"
                "# > delete name - delete the 'name' function                    #\n"
                "# > clear       - delete all functions                          #\n"
                "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
                "# > grammar    - all available operators and constants          #\n"
                "# > args-info  - guide on args                                  #\n"
                "# > help       - general app usage guide                        #\n"
                "# > exit       - terminate the program                          #\n"
                "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
        );
        cout << message;
    }

    ~Calculator()
    {
        delete _compiler;
    }
};


#endif //INC_4_FUNCTIONS_CALCULATOR_HPP

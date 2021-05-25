#include "function.h"

Function::Exception::Exception (
            string && info_,
            string && file_,
            string && scope_,
            const size_t line_
        ) : info(info_), scope(scope_), file(file_), line(line_) {}

bool Function::overrideRegisters = false;
bool Function::enableConstants = true;

std::map <string, double> Function::constants;
std::map <string, Function::unary_operation_t> Function::prefixFunctions;
std::map <string, Function::BinaryOperation> Function::binaryOperations;
std::map <string, Function::unary_operation_t> Function::postfixFunctions;

bool Function::registerConstant (string && signature, const double value)
{
    if (overrideRegisters && Function::constants.contains (signature)) return false;
    else Function::constants[signature] = value;
    return true;
}
bool Function::registerPrefixFunction (string && signature, const unary_operation_t function)
{
    if (overrideRegisters && Function::prefixFunctions.contains (signature)) return false;
    else Function::prefixFunctions[signature] = function;
    return true;
}
bool Function::registerBinaryOperation (string && signature, const binary_operation_t function, const precedence_t precedence)
{
    if (overrideRegisters && Function::binaryOperations.contains (signature)) return false;
    else Function::binaryOperations[signature] = BinaryOperation {
        .precedence = precedence,
        .function = function
    };
    return true;
}
bool Function::registerPostfixFunction (string && signature, const unary_operation_t function)
{
    if (overrideRegisters && Function::postfixFunctions.contains (signature)) return false;
    else Function::postfixFunctions[signature] = function;
    return true;
}

const std::map<string, double> & Function::allConstants ()
{
    return Function::constants;
}
const std::map<string, Function::unary_operation_t> & Function::allPrefix ()
{
    return Function::prefixFunctions;
}
const std::map<string, Function::BinaryOperation> & Function::allBinary ()
{
    return Function::binaryOperations;
}
const std::map<string, Function::unary_operation_t> & Function::allPostfix ()
{
    return Function::postfixFunctions;
}

void Function::basic_init ()
{
    registerConstant ("pi", M_PI);
    registerConstant ("e", M_E);

    registerPrefixFunction ("-", [](const double x) -> double {return -x;});
    registerPrefixFunction ("exp", [](const double x) -> double {return std::exp (x);});
    registerPrefixFunction ("sin", [](const double x) -> double {return std::sin (x);});
    registerPrefixFunction ("cos", [](const double x) -> double {return std::cos (x);});
    registerPrefixFunction ("floor", [](const double x) -> double {return std::floor (x);});
    registerPrefixFunction ("ceil", [](const double x) -> double {return std::ceil (x);});
    registerPrefixFunction ("round", [](const double x) -> double {return std::round (x);});

    registerBinaryOperation ("+",[](const double a, const double b) -> double { return a + b; },1);
    registerBinaryOperation ("-",[](const double a, const double b) -> double { return a - b; },1);
    registerBinaryOperation ("*",[](const double a, const double b) -> double { return a * b; },2);
    registerBinaryOperation ("/",[](const double a, const double b) -> double { return a / b; },2);
    registerBinaryOperation ("^", pow, 3 );

    registerPostfixFunction ("!", [](const double x) -> double {
        auto n = (size_t)x;
        size_t result = 1;
        while (n > 1)
            result *= n--;
        return (double) result;
    });
}

size_t Function::matchNumber (const string & instance, const size_t start)
{
    bool isSigned = (instance[start] == '-' || instance[start] == '+');
    size_t length = isSigned;
    while (isdigit (instance[start+length])) ++length;
    length += instance[start+length] == '.';
    while (isdigit (instance[start+length])) ++length;
    return length * (!isSigned || length > 1);
}
size_t Function::matchArgument (const string & instance, const size_t start)
{
    size_t length=0;
    while (isalpha(instance[start+length]) || instance[start+length] == '_') ++length;
    return length;
}
size_t Function::matchPrefixFunction (const string & instance, const size_t start)
{
    size_t length;
    for (const auto & pair: Function::prefixFunctions)
    {
        length = pair.first.length();
        if (start + length <= instance.length() && instance.compare (start, length, pair.first) == 0)
        {
            return length;
        }
    }
    return 0;
}
size_t Function::matchBinaryOperator (const string & instance, const size_t start)
{
    size_t length;
    for (const auto & pair: Function::binaryOperations)
    {
        length = pair.first.length();
        if (start + length <= instance.length() && instance.compare (start, length, pair.first) == 0)
        {
            return length;
        }
    }
    return 0;
}
size_t Function::matchPostfixFunction (const string & instance, const size_t start)
{
    size_t length;
    for (const auto & pair: Function::postfixFunctions)
    {
        length = pair.first.length();
        if (start + length <= instance.length() && instance.compare (start, length, pair.first) == 0)
        {
            return length;
        }
    }
    return 0;
}

Function::precedence_t Function::precedence_of (const string & signature)
{
    auto lookup = Function::binaryOperations.find (signature);
    return (lookup != Function::binaryOperations.end ()) ? lookup->second.precedence : 0;
}

Function::Function (string infix)
{
    // PREFIX       -> number|argument|prefix|opening
    // BINARY       -> number|argument|prefix|opening
    // OPENING      -> number|argument|prefix|opening
    // NUMBER       -> binary|postfix|closing
    // ARGUMENT     -> binary|postfix|closing
    // POSTFIX      -> binary|postfix|closing
    // CLOSING      -> binary|postfix|closing
    // left: --*--** | right: -*-**---
    static const auto leftUnitGroup = static_cast<UnitType> (UT_NUMBER|UT_ARGUMENT|UT_PREFIX_FUNCTION|UT_OPENING_BRACKET);
    static const auto rightUnitGroup = static_cast<UnitType> (UT_BINARY_OPERATOR|UT_POSTFIX_FUNCTION|UT_CLOSING_BRACKET);

    UnitType unitGroup = leftUnitGroup;
    this->_infix = infix;

    Stack<Function::Unit> stack;

    size_t length, i=0;
    while (infix[i] == ' ') ++i;
    while (i < infix.size ())
    {
        if ((UT_NUMBER & unitGroup) && (length = matchNumber (infix, i)))
        {
            this->_expression.push_back (Unit {
                .type = UT_NUMBER,
                .value = infix.substr (i, length)
            });
            std::cout << "UT_NUMBER; length=" << length << "; value={" << this->_expression.back ().value << "}\n";
//            unitGroup = afterNumberOrArgument;
            unitGroup = rightUnitGroup;
        }
        /*else if ((UT_NEGATION & unitGroup) && (length = (infix[i] == '-')))
        {
            std::cout << "UT_NEGATION\n";
            stack.push (Unit {
                .type = UT_PREFIX_FUNCTION,
                .value = "-"
            });
            unitGroup = afterNegation;
        }*/
        else if ((UT_PREFIX_FUNCTION & unitGroup) && (length = matchPrefixFunction (infix, i)))
        {
//            std::cout << "UT_PREFIX_FUNCTION\n";
            stack.push (Unit {
               .type = UT_PREFIX_FUNCTION,
               .value = infix.substr (i, length)
            });
//            unitGroup = afterPrefixFunction;
            unitGroup = leftUnitGroup;
        }
        else if ((UT_BINARY_OPERATOR & unitGroup) && (length = matchBinaryOperator (infix, i)))
        {
//            std::cout << "UT_BINARY_OPERATOR\n";
            string signature = infix.substr (i, length);
            precedence_t precedence = precedence_of (signature);
            while (!stack.empty () && (stack.top ().type == UT_PREFIX_FUNCTION || precedence_of (stack.top ().value) > precedence))
            {
                this->_expression.push_back (stack.top ());
                stack.pop ();
            }
            stack.push (Unit {
                .type = UT_BINARY_OPERATOR,
                .value = signature
            });
//            unitGroup = afterBinaryOperator;
            unitGroup = leftUnitGroup;
        }
        else if ((UT_POSTFIX_FUNCTION & unitGroup) && (length = matchPostfixFunction (infix, i)))
        {
//            std::cout << "UT_POSTFIX_FUNCTION\n";
            this->_expression.push_back (Unit {
               .type = UT_POSTFIX_FUNCTION,
               .value = infix.substr (i, length)
            });
            unitGroup = rightUnitGroup;
//            unitGroup = afterPostfixFunction;
        }
        else if ((UT_OPENING_BRACKET & unitGroup) && (length = infix[i] == '('))
        {
//            std::cout << "UT_OPENING_BRACKET\n";
            stack.push (Unit { .type = UT_OPENING_BRACKET });
            unitGroup = leftUnitGroup;
//            unitGroup = afterOpeningBracket;
        }
        else if ((UT_CLOSING_BRACKET & unitGroup) && (length = (infix[i] == ')')))
        {
//            std::cout << "UT_CLOSING_BRACKET\n";
            while (stack.top().type != UT_OPENING_BRACKET)
            {
                this->_expression.push_back (stack.top ());
                stack.pop ();
            }
            stack.pop ();
            unitGroup = rightUnitGroup;
//            unitGroup = afterClosingBracket;
        }
        else if ((UT_ARGUMENT & unitGroup) && (length = matchArgument (infix, i)))
        {
//            std::cout << "UT_ARGUMENT\n";
            this->_expression.push_back (Unit {
               .type = UT_ARGUMENT,
               .value = infix.substr (i, length)
            });
            unitGroup = rightUnitGroup;
//            unitGroup = afterNumberOrArgument;
        }
        else
        {
            std::cout << "Unhandled variant\n";
            std::printf ("i=%z\n", i);
            std::bitset<8> b (unitGroup);
            std::cout << "unitGroup: " << b << std::endl;
            break;
        }
        i += length;
        while (infix[i] == ' ') ++i;
    }
    while (!stack.empty ())
    {
        this->_expression.push_back (stack.top ());
        stack.pop ();
    }
#ifdef OLD_ALGORITHM
    if ( infix[i] == '+' ) ++i;
    else if (infix[i] == '-')
    {
        offset = 1;
        while (infix[i+offset] == ' ') ++offset;
        if (!isdigit (infix[i+offset]))
        {
            stack.push (
                Unit {
                    .type = UT_PREFIX_FUNCTION,
                    .value = "~"
                }
            );
            ++i;
        }
        else
        {
            matchNumber (infix, i+offset, length);
            this->_expression.push_back (Unit {
                .type = UT_NUMBER,
                .value = ((offset < 2) ? infix.substr (i, length+offset) : ("-" + infix.substr (i+offset, length)))
            });
            i += length;
        }
        while (infix[i] == ' ') ++i;
    }

    for (;i < infix.length (); ++i)
    {
        length = 0;

        if (infix[i] == '(')
        {
            length = 0;
            stack.push (Unit {.type = UT_OPENING_BRACKET});
            ++i;
            while (infix[i] == ' ') ++i;
            if ( infix[i] == '+' ) ++i;
            else if (infix[i] == '-')
            {
                offset = 1;
                while (infix[i+offset] == ' ') ++offset;
                if (!isdigit(infix[i+offset]))
                {
                    stack.push (Unit {
                        .type = UT_PREFIX_FUNCTION,
                        .value = "~"
                    });
                    ++i;
                }
                else
                {
                    matchNumber(infix, i+offset, length);
                    this->_expression.push_back (Unit {
                       .type = UT_NUMBER,
                       .value = ((offset < 2) ? infix.substr (i, length+offset) : ("-" + infix.substr (i+offset, length)))
                    });
                    std::cout << "length: " << length;
                    i += offset + 1;
                }
            }
            --i;
        }
        else if (matchPrefixFunction (infix, i, length))
        {
            stack.push (Unit {
                .type = UT_PREFIX_FUNCTION,
                .value = infix.substr (i, length)
            });
        }
        else if (matchPostfixFunction (infix, i, length))
        {
            this->_expression.push_back (Unit {
               .type = UT_POSTFIX_FUNCTION,
               .value = infix.substr (i, length)
            });
        }
        else if (matchBinaryOperator (infix, i, length))
        {
            string signature = infix.substr (i, length);
            precedence_t precedence = precedence_of (signature);
            while (!stack.empty () && (stack.top ().type == UT_PREFIX_FUNCTION || precedence_of (stack.top ().value) > precedence))
            {
                this->_expression.push_back (stack.top ());
                stack.pop ();
            }
            stack.push (Unit {
                .type = UT_BINARY_OPERATOR,
                .value = signature
            });
        }
        else if (matchNumber (infix, i, length))
        {
            this->_expression.push_back (Unit {
                .type = UT_NUMBER,
                .value = infix.substr (i, length)
            });
        }
        else if (matchArgument (infix, i, length))
        {
            this->_expression.push_back (Unit {
                .type = UT_ARGUMENT,
                .value = infix.substr (i, length)
            });
        }
        else if (infix[i] == ')')
        {
            length = 1;
            while (stack.top ().type != UT_OPENING_BRACKET)
            {
                this->_expression.push_back (stack.top ());
                stack.pop ();
            }
            stack.pop ();
        }
        i += length - 1;
        while (infix[i] == ' ') ++i;
    }
#endif
}
Function::Function (const Function & instance)
{
    this->_infix = instance._infix;
    for (const auto & unit : instance._expression)
    {
        this->_expression.push_back (Unit {
            .type = unit.type,
            .value = unit.value
        });
    }
}

double Function::evaluate (const args_t & args) const
{
    Stack<double> stack;
    double a, b;
    std::_Rb_tree_iterator<std::pair<const std::basic_string<char>, double>> lookup;
    for (const auto & unit : this->_expression)
    {
        switch (unit.type)
        {
            case UT_NUMBER:
                stack.push (stod (unit.value));
                break;
            case UT_ARGUMENT:
                if (enableConstants)
                {
                    lookup = constants.find (unit.value);
                    if (lookup != constants.end ())
                    {
                        stack.push (lookup->second);
                        break;
                    }
                }
                stack.push (args.at (unit.value));
                break;
            case UT_PREFIX_FUNCTION:
                a = stack.top ();
                stack.pop ();
                stack.push (Function::prefixFunctions.at (unit.value) (a));
                break;
            case UT_POSTFIX_FUNCTION:
                a = stack.top ();
                stack.pop ();
                stack.push (Function::postfixFunctions.at (unit.value) (a));
                break;
            case UT_BINARY_OPERATOR:
                a = stack.top (); stack.pop ();
                b = stack.top (); stack.pop ();
                stack.push (Function::binaryOperations.at(unit.value).function (b, a));
                break;
            default:
                std::cout << "Unhandled unit type\n";
                throw Function::Exception (
                    "Unhandled unit type: Unit{.type=" + std::to_string (static_cast<unsigned>(unit.type)) + ", .value=\"" + unit.value + "\"}",
                    "function.cpp",
                    "Function::evaluate::for::switch::default",
                    236
                );
        }
    }
    return stack.top ();
}
double Function::operator () (const args_t & args) const
{
    return this->evaluate (args);
}
double Function::evaluate () const
{
    /*if (!this->_argsNames.empty ())
    {
        std::cout << "argless call was canceled\n";
        throw Function::Exception (
            "ERROR: argless call was canceled, as the function contains arguments",
            "function.cpp",
            "Function::evaluate::if",
            286
        );
    }*/
    args_t args;
    return this->evaluate (args);
}
double Function::operator () () const
{
    return this->evaluate();
}

string Function::asString () const
{
    size_t size = 0;
    for (const auto & unit : this->_expression)
        size += unit.value.size () + 1 + (unit.type == UT_NONE);
    string s;
    s.reserve (size - 1);
    auto unitIt = this->_expression.cbegin ();
    s += unitIt++->value;
    for (; unitIt != this->_expression.cend (); ++unitIt)
    {
        s += ' ';
        if (unitIt->type == UT_NONE) s += '-';
        s += unitIt->value;
    }
    return s;
}
const string& Function::infix () const
{
    return this->_infix;
}
// -= ==---------------== =-
// -= =-- DEFINE-AREA --= =-
#define USE_HANDMADE_STACK
// -= ==---------------== =-



// -= ==----------------== =-
// -= =-- INCLUDE-AREA --= =-
#include <regex>

#include "function.h"
// -= ==----------------== =-



// -= ==--------------== =-
// -= =-- USING-AREA --= =-
using std::map;

using std::string;

using std::cin;
using std::cout;
using std::endl;

using std::regex;
using std::sregex_iterator;
using std::smatch;
using std::regex_match;
// -= ==--------------== =-



// -= ==----------------== =-
// -= =-- TYPEDEF-AREA --= =-
typedef void (*commandArgless_t) ();
typedef void (*commandSingleArg_t) (const string&);
// -= ==----------------== =-



// -= ==----------------== =-
// -= =-- GLOBALS-AREA --= =-
map <string, Function> environment;
// -= ==----------------== =-



// -= ==------------------== =-
// -= =-- FUNCTIONS-AREA --= =-
void exists (const string & name)
{
    cout << ((environment.contains (name))
        ? "Such function exists\n"
        : "There is no such function\n");
}
void set (const string & name, const string & pattern)
{
    environment.insert_or_assign (name, Function (pattern));
}
void evaluate (const string & name, const Function::args_t & args)
{
    auto lookup = environment.find (name);
    if (lookup != environment.end ())
        cout << "The result is: " << lookup->second (args) << endl;
    else
        cout << "There is no function with name '" << name << "' in environment\n";
}
void erase (const string & name)
{
    environment.erase (name);
}
void clear ()
{
    environment.clear ();
}
void show (const string & name)
{
    auto lookup = environment.find (name);
    if (lookup != environment.end ())
        cout << "Infix form: " << lookup->second.infix () << "\nPostfix form: " << lookup->second.asString () << endl;
}
void show ()
{
    if (environment.empty ())
        cout << "The environment is currently empty\n";
    else
    {
        for (const auto &item : environment)
            cout << item.first << ": " << item.second.infix () << endl;
    }
}
void help ()
{
    static const string message (
        "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
        "# AVAILABLE COMMANDS:                                                     #\n"
        "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
        "# > @set name function   - save the function to the environment as 'name' #\n"
        "# > @eval name args...   - evaluate 'name' function with given args       #\n"
        "# > @do function args... - evaluate function with given args              #\n"
        "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
        "# > @show name  - show the 'name' function in infix notation              #\n"
        "# > @show       - show all functions in the environment                   #\n"
        "# > @erase name - delete the 'name' function from the environment         #\n"
        "# > @clear      - delete all functions from the environment               #\n"
        "#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#\n"
        "# > @args-info  - show guide on args                                      #\n"
        "# > @list       - show all available functions, operations and constants  #\n"
        "# > @help       - show this message again                                 #\n"
        "# > @exit       - terminate the program                                   #\n"
        "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n"
    );
    cout << message;
}
void args_info ()
{
    static const string message (
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
void defined ()
{
    cout << "Constants: ";
    for (const auto & pair : Function::allConstants())
        cout << pair.first << '=' << pair.second << ", ";
    cout << "\nPrefix Functions: ";
    for (const auto & pair : Function::allPrefix())
        cout << pair.first << ", ";
    cout << "\nBinary Operations (precedence): ";
    for (const auto & pair : Function::allBinary())
        cout << pair.first << '(' << (short)pair.second.precedence << "), ";
    cout << "\nPostfix Functions: ";
    for (const auto & pair : Function::allPostfix())
        cout << pair.first << ", ";
    cout << endl;
}
// -= ==------------------== =-



int main ()
{

    Function::basic_init ();

    const map<string, commandArgless_t> arglessCommands {
        { "clear", clear },
        { "show", show },
        {"help", help },
        {"args-info", args_info },
        {"list", defined }
    };
    const map<string, commandSingleArg_t> singleArgCommands {
        { "erase", erase },
        { "exists", exists },
        { "show", show }
    };
    const regex commandExitPattern {"^exit\\s*$"};
    const regex commandArglessPattern {"^(clear|show|help|args-info|list)\\s*$"};
    const regex commandSingleArgPattern {"^(erase|exists|show)\\s+([a-zA-Z_]+)\\s*$"};
    const regex commandSetPattern {R"(^set\s+([a-zA-Z_]+)\s+([()a-zA-Z0-9.+\-*/^! ]+)\s*$)"};
    const regex commandDoPattern {R"(^do\s+([()a-zA-Z0-9.+\-*/^! ]+)((?:\s+[a-zA-Z]+=[\-0-9.]+)*)\s*$)"};
    const regex commandEvalPattern {R"(^eval\s+([a-zA-Z_]+)((?:\s+[a-zA-Z]+=[\-0-9.]+)*))"};
    const regex functionArgPattern {"([a-zA-Z]+)=([\\-0-9.]+)"};

    bool running = true;
    string input;
    smatch match;
    help ();
    cout << endl;
    while (running)
    {
        cout << '@';
        getline (cin, input);
        if (regex_match (input, commandExitPattern))
            running = false;
        else
        {
            if (regex_match (input, match, commandArglessPattern))
                arglessCommands.at (match[1].str ()) ();
            else if (regex_match (input, match, commandSingleArgPattern))
                singleArgCommands.at (match[1].str ()) (match[2].str ());
            else if (regex_match (input, match, commandSetPattern))
                set (match[1].str (), match[2].str ());
            else if (regex_match (input, match, commandDoPattern))
            {
                Function temp {match[1].str ()};
                Function::args_t args;
                string argsString = match[2].str ();
                auto iterator = sregex_iterator (argsString.begin (), argsString.end (), functionArgPattern);
                auto end = sregex_iterator ();
                for (; iterator != end; ++iterator)
                    args[(*iterator)[1].str ()] = stod ((*iterator)[2].str ());
                double result = temp (args);
                cout << "The result is: " << result << endl;
            }
            else if (regex_match (input, match, commandEvalPattern))
            {
                auto lookup = environment.find (match[1].str ());
                if (lookup != environment.end ())
                {
                    Function::args_t args;
                    string argsString = match[2].str ();
                    auto iterator = sregex_iterator (argsString.begin (), argsString.end (), functionArgPattern);
                    auto end = sregex_iterator ();
                    for (; iterator != end; ++iterator)
                        args[(*iterator)[1].str ()] = stod ((*iterator)[2].str ());
                    double result = lookup->second (args);
                    cout << "The result is: " << result << endl;
                }
                else
                    cout << "There is no function with name '" << match[1].str () << '\'' << endl;
            }
        }
        cout << endl;
    }
    return 0;
}

#include "macroses.h"
#include <string>

using namespace std;

string UpcaseString(const string& arg_string)
{
    string result;
    for (char c : arg_string)
        result += toupper(c);
    return result;
}

string TrimString(const string& arg_string)
{
    string result;
    size_t begin_not_space = arg_string.find_first_not_of(" \t\r\n"s);
    size_t end_not_space = arg_string.find_last_not_of(" \t\r\n"s);
    if (begin_not_space != string::npos && end_not_space != string::npos)
        result = arg_string.substr(begin_not_space, end_not_space - begin_not_space + 1);
    return result;
}

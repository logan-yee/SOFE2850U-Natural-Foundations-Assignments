// SOFE 2850U - Assignment 1
// Logan Yee - 100833629
//
// Base conversion calculator: converts a whole number from any base (2-36)
// to any other base. Digits above 9 are written as letters (A = 10, B = 11, ... Z = 35), upper or lower case.


#include <algorithm>
#include <cctype>
#include <climits>
#include <iostream>
#include <string>

using namespace std;

const int MIN_BASE = 2;
const int MAX_BASE = 36;

// Returns the value of a digit character (0-35), or -1 if it is not a digit.
int CharToDigit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    c = toupper(static_cast<unsigned char>(c));
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return -1;
}

// Returns the character used to print a digit value (0-35).
char DigitToChar(int digit)
{
    if (digit < 10)
        return '0' + digit;
    return 'A' + (digit - 10);
}

// Removes spaces and tabs from both ends of a string.
string Trim(const string &text)
{
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Checks that the text is a whole number between MIN_BASE and MAX_BASE.
// On success the base is stored in 'base'.
bool ValidateBase(const string &text, int &base)
{
    if (text.empty() || text.length() > 2)
        return false;
    for (size_t i = 0; i < text.length(); i++)
    {
        if (!isdigit(static_cast<unsigned char>(text[i])))
            return false;
    }
    base = stoi(text);
    return base >= MIN_BASE && base <= MAX_BASE;
}

// Checks that the input consists only of characters that are legal in the
// source base (e.g. only 0 and 1 for binary). One leading '-' is allowed.
bool ValidateInput(const string &input_number, int source_base)
{
    size_t start = (!input_number.empty() && input_number[0] == '-') ? 1 : 0;
    if (start == input_number.length())
        return false; // empty, or a '-' with no digits after it

    for (size_t i = start; i < input_number.length(); i++)
    {
        int digit = CharToDigit(input_number[i]);
        if (digit < 0 || digit >= source_base)
            return false;
    }
    return true;
}

// Step 1: source base -> value. Each digit is folded in from left to right:
// value = value * base + digit. Returns false if the number is too large to
// store.
bool ToDecimal(const string &digits, int source_base, unsigned long long &value)
{
    value = 0;
    for (size_t i = 0; i < digits.length(); i++)
    {
        unsigned long long digit = CharToDigit(digits[i]);
        if (value > (ULLONG_MAX - digit) / source_base)
            return false;
        value = value * source_base + digit;
    }
    return true;
}

// Step 2: value -> target base. Repeatedly divide by the base; the
// remainders are the digits, produced from least to most significant.
string FromDecimal(unsigned long long value, int target_base)
{
    if (value == 0)
        return "0";

    string digits;
    while (value > 0)
    {
        digits += DigitToChar(value % target_base);
        value /= target_base;
    }
    reverse(digits.begin(), digits.end());
    return digits;
}

// Converts input_number from source_base to target_base. The result is
// returned as a string because bases above 10 need letter digits, which an
// int cannot hold. Returns an empty string if the number is too large.
// The input must already have passed ValidateInput.
string convert_number(const string &input_number, int source_base, int target_base)
{
    bool negative = input_number[0] == '-';
    string digits = negative ? input_number.substr(1) : input_number;

    unsigned long long value;
    if (!ToDecimal(digits, source_base, value))
        return "";

    string result = FromDecimal(value, target_base);
    if (negative && value != 0)
        result = "-" + result;
    return result;
}

// Prints a prompt and reads one line. Returns false if the user asked to
// quit (entered Q) or the input stream ended.
bool ReadInput(const string &prompt, string &input)
{
    cout << prompt;
    if (!getline(cin, input))
        return false;
    input = Trim(input);
    return !(input == "q" || input == "Q");
}

// Keeps asking for a base until a valid one is entered. Returns false if
// the user quits.
bool ReadBase(const string &prompt, int &base)
{
    string input;
    while (true)
    {
        if (!ReadInput(prompt, input))
            return false;
        if (ValidateBase(input, base))
            return true;
        cout << "  Invalid base. Enter a whole number from " << MIN_BASE
             << " to " << MAX_BASE << ".\n";
    }
}

void PrintInstructions()
{
    cout << "=============================================\n";
    cout << "          BASE CONVERSION CALCULATOR\n";
    cout << "=============================================\n";
    cout << "Converts a whole number from any base to any\n";
    cout << "other base (bases " << MIN_BASE << " to " << MAX_BASE << ").\n\n";
    cout << " - Use letters for digits above 9: A = 10, B = 11, ... Z = 35\n";
    cout << " - Common bases: 2 = binary, 8 = octal, 10 = decimal, 16 = hexadecimal\n";
    cout << " - Enter Q at any prompt to quit\n";
    cout << "   (to convert the single digit Q, enter it as 0Q)\n";
}

int main()
{
    PrintInstructions();

    while (true)
    {
        string input_number;
        int source_base, target_base;

        cout << "\n";
        if (!ReadInput("Enter the number to convert: ", input_number))
            break;
        if (input_number.empty())
            continue;
        if (!ReadBase("Enter the source base (convert FROM): ", source_base))
            break;

        if (!ValidateInput(input_number, source_base))
        {
            cout << "  \"" << input_number << "\" is not a valid base " << source_base
                 << " number. Legal digits are 0 to " << DigitToChar(source_base - 1)
                 << ". Please try again.\n";
            continue;
        }

        if (!ReadBase("Enter the target base (convert TO): ", target_base))
            break;

        string result = convert_number(input_number, source_base, target_base);
        if (result.empty())
        {
            cout << "  That number is too large for this calculator. Please try again.\n";
            continue;
        }

        cout << "\n  " << input_number << " (base " << source_base << ") = "
             << result << " (base " << target_base << ")\n";

        string answer;
        if (!ReadInput("\nPress Enter to convert another number, or Q to quit: ", answer))
            break;
    }

    cout << "\nGoodbye!\n";
    return 0;
}

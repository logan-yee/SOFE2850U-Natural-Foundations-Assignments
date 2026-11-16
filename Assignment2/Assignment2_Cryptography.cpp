// SOFE 2850U - Assignment 2
// Logan Yee - 100833629
//
// Cryptographic Techniques Program: encrypts and decrypts messages with either
// a Caesar cipher or a Vigenere cipher.
//
// Caesar cipher: every character from '0' (ASCII 48) to 'z' (ASCII 122) is
// shifted by a fixed key of 7, wrapping around inside that 75-character range.
// Characters outside the range (such as spaces) are left unchanged.
//
// Vigenere cipher: each letter is shifted by the matching letter of a keyword
// (a = 0, b = 1, ... z = 25). The keyword repeats over the message and only
// advances on letters, so spaces, digits and punctuation are kept as they are.
// Upper and lower case letters keep their case.

#include <iostream>
#include <string>

using namespace std;

// Caesar cipher settings
const int CAESAR_KEY = 7;
const char CAESAR_FIRST = '0';                           // ASCII 48
const char CAESAR_LAST = 'z';                            // ASCII 122
const int CAESAR_RANGE = CAESAR_LAST - CAESAR_FIRST + 1; // 75 characters

// Vigenere cipher settings
const int ALPHABET_SIZE = 26;

// Removes spaces, tabs and line-ending characters from both ends of a string.
string Trim(const string &text)
{
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool IsLower(char c)
{
    return c >= 'a' && c <= 'z';
}

bool IsUpper(char c)
{
    return c >= 'A' && c <= 'Z';
}

// Caesar cipher 

// Encrypts with E = (C - '0' + k) % 75 + '0'.
// C - '0' is the character's position in the range (0-74). Adding k shifts it,
// and % 75 wraps positions past 'z' back around to '0'.
string CaesarEncrypt(const string &message, int key)
{
    string result = message;
    for (size_t i = 0; i < result.length(); i++)
    {
        char c = result[i];
        if (c >= CAESAR_FIRST && c <= CAESAR_LAST)
            result[i] = (c - CAESAR_FIRST + key) % CAESAR_RANGE + CAESAR_FIRST;
    }
    return result;
}

// Decrypts with D = (E - '0' - k + 75) % 75 + '0', which undoes the encryption
// formula. Subtracting k shifts the character back; the + 75 keeps the value
// from going negative when the shift wraps back past '0' (in C++, % of a
// negative number gives a negative result).
string CaesarDecrypt(const string &message, int key)
{
    string result = message;
    for (size_t i = 0; i < result.length(); i++)
    {
        char c = result[i];
        if (c >= CAESAR_FIRST && c <= CAESAR_LAST)
            result[i] = (c - CAESAR_FIRST - key + CAESAR_RANGE) % CAESAR_RANGE + CAESAR_FIRST;
    }
    return result;
}

// Vigenere cipher 
// Returns 'a' for a lower case letter, 'A' for an upper case letter, or 0 if
// the character is not a letter. Subtracting this base from a letter gives its
// position in the alphabet (0-25).
char LetterBase(char c)
{
    if (IsLower(c))
        return 'a';
    if (IsUpper(c))
        return 'A';
    return 0;
}

// Returns the shift k (0-25) for the n-th letter of the message. The key is
// repeated over the message: key[0], key[1], ..., then back to key[0].
// A key letter's ASCII value (97-122) minus 'a' gives its shift.
int KeyShift(const string &key, size_t n)
{
    return key[n % key.length()] - 'a';
}

// Encrypts with E = (C + k) % 26, where C and k are letter positions (0-25).
// Non-letters are copied unchanged and do not use up a key letter.
string VigenereEncrypt(const string &message, const string &key)
{
    string result = message;
    size_t letters = 0;
    for (size_t i = 0; i < result.length(); i++)
    {
        char base = LetterBase(result[i]);
        if (base == 0)
            continue;
        int C = result[i] - base;
        int k = KeyShift(key, letters++);
        result[i] = (C + k) % ALPHABET_SIZE + base;
    }
    return result;
}

// Decrypts with D = (E - k + 26) % 26, where E and k are letter positions (0-25).
// The + 26 keeps the value from going negative before taking the remainder.
string VigenereDecrypt(const string &message, const string &key)
{
    string result = message;
    size_t letters = 0;
    for (size_t i = 0; i < result.length(); i++)
    {
        char base = LetterBase(result[i]);
        if (base == 0)
            continue;
        int E = result[i] - base;
        int k = KeyShift(key, letters++);
        result[i] = (E - k + ALPHABET_SIZE) % ALPHABET_SIZE + base;
    }
    return result;
}

// Input

// Prints a prompt and reads one line into 'input'. Returns false if the input
// stream ended.
bool ReadLine(const string &prompt, string &input)
{
    cout << prompt;
    if (!getline(cin, input))
        return false;
    if (!input.empty() && input[input.length() - 1] == '\r')
        input.erase(input.length() - 1);
    return true;
}

// Shows a two-option menu until the user enters 1 or 2.
// Returns the choice, or 0 if the input stream ended.
int ReadMenuChoice(const string &menu)
{
    string input;
    while (true)
    {
        if (!ReadLine(menu, input))
            return 0;
        input = Trim(input);
        if (input == "1" || input == "2")
            return input[0] - '0';
        cout << "  Invalid selection. Please enter 1 or 2.\n";
    }
}

// Reads the message to encrypt or decrypt. Spaces in the message are kept.
// Returns false if the input stream ended.
bool ReadMessage(const string &prompt, string &message)
{
    while (true)
    {
        if (!ReadLine(prompt, message))
            return false;
        if (!Trim(message).empty())
            return true;
        cout << "  The message cannot be empty. Please try again.\n";
    }
}

// Reads a Vigenere key made of the letters a-z. Upper case letters are
// accepted and converted to lower case. Returns false if the input stream ended.
bool ReadVigenereKey(string &key)
{
    while (true)
    {
        if (!ReadLine("Enter the Vigenere key (letters a-z only): ", key))
            return false;
        key = Trim(key);

        bool valid = !key.empty();
        for (size_t i = 0; i < key.length() && valid; i++)
        {
            if (IsUpper(key[i]))
                key[i] = key[i] - 'A' + 'a';
            valid = IsLower(key[i]);
        }
        if (valid)
            return true;
        cout << "  Invalid key. Use letters only (a-z), with no spaces, digits or symbols.\n";
    }
}

// Asks whether to continue (C) or exit (E). Returns true to continue.
bool AskToContinue()
{
    string input;
    while (true)
    {
        if (!ReadLine("\nEnter C to continue or E to exit: ", input))
            return false;
        input = Trim(input);
        if (input == "C" || input == "c")
            return true;
        if (input == "E" || input == "e")
            return false;
        cout << "  Invalid choice. Please enter C or E.\n";
    }
}

//Main

int main()
{
    cout << "Welcome to Cryptographic Techniques Program\n";

    do
    {
        int action = ReadMenuChoice("\nPlease enter your selection:\n"
                                    "1. Encrypt\n"
                                    "2. Decrypt\n"
                                    "Selection: ");
        if (action == 0)
            break;
        bool encrypt = (action == 1);

        int cipher = ReadMenuChoice(string("\n") + (encrypt ? "Encrypt" : "Decrypt") + ":\n"
                                    "Which cipher to use?\n"
                                    "1. Caesar cipher\n"
                                    "2. Vigenere cipher\n"
                                    "Selection: ");
        if (cipher == 0)
            break;

        string message;
        if (!ReadMessage(encrypt ? "\nEnter the message to encrypt: "
                                 : "\nEnter the message to decrypt: ",
                         message))
            break;

        string result;
        if (cipher == 1)
        {
            cout << "Using Caesar cipher key k = " << CAESAR_KEY << "\n";
            result = encrypt ? CaesarEncrypt(message, CAESAR_KEY)
                             : CaesarDecrypt(message, CAESAR_KEY);
        }
        else
        {
            string key;
            if (!ReadVigenereKey(key))
                break;
            result = encrypt ? VigenereEncrypt(message, key)
                             : VigenereDecrypt(message, key);
        }

        if (encrypt)
        {
            cout << "\nOriginal Message: " << message << "\n";
            cout << "Encrypted Message: " << result << "\n";
        }
        else
        {
            cout << "\nEncrypted Message: " << message << "\n";
            cout << "Decrypted Message: " << result << "\n";
        }
    } while (AskToContinue());

    cout << "\nGoodbye\n";
    return 0;
}

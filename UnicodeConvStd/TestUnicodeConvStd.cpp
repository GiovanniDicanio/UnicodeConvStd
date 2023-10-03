////////////////////////////////////////////////////////////////////////////////
// TestUnicodeConvStd.cpp : Test the Unicode conversion functions
// by Giovanni Dicanio <giovanni.dicanio AT gmail.com>
////////////////////////////////////////////////////////////////////////////////


#include "UnicodeConvStd.hpp"   // Module to test

#include <crtdbg.h>             // _ASSERTE

#include <iostream>             // For console output
#include <string>               // std::string, std::wstring


// Convenient function to print PASSED/FAILED on a single test,
// alongside a short description for the test
void Check(bool condition, const char* description)
{
    std::cout << "[" << description << "]: ";
    if (condition)
    {
        std::cout << "PASSED\n";
    }
    else
    {
        std::cout << "FAILED\n";
    }
}


//
// Various Tests
//

void TestEmptyStrings()
{
    std::wstring utf16empty;
    std::string utf8empty = UnicodeConvStd::ToUtf8(utf16empty);
    _ASSERTE(utf8empty.empty());
    Check(utf8empty.empty(), "Empty strings");
}


void TestStringsWithJapaneseKanji()
{
    // Unicode character U+5B66 (Japanese kanji meaning "learn, study")
    // https://www.compart.com/en/unicode/U+5B66
    //
    // UTF-16 encoding: 0x5B66
    // UTF-8 encoding: 0xE5 0xAD 0xA6

    std::wstring utf16 = L"Japanese kanji \x5B66";
    std::string utf8 = UnicodeConvStd::ToUtf8(utf16);
    std::wstring utf16Again = UnicodeConvStd::ToUtf16(utf8);
    _ASSERTE(utf16 == utf16Again);
    Check(utf16 == utf16Again, "String with Japanese kanji");
}


void TestStringLengths()
{
    // Unicode character U+5B66 (Japanese kanji meaning "learn, study")
    // https://www.compart.com/en/unicode/U+5B66
    //
    // UTF-16 encoding: 0x5B66
    // UTF-8 encoding: 0xE5 0xAD 0xA6

    std::wstring utf16 = L"\x5B66";
    _ASSERTE(utf16.length() == 1); // 1 wchar_t in UTF-16

    std::string utf8 = UnicodeConvStd::ToUtf8(utf16);
    _ASSERTE(utf8.length() == 3); // 3 chars when encoded in UTF-8
    Check(utf8.length() == 3, "UTF-8 length");
    _ASSERTE(strlen(utf8.c_str()) == 3);

    const BYTE utf8Encoding[] = { 0xE5, 0xAd, 0xA6 };
    bool matchingBytes = memcmp(utf8Encoding, utf8.c_str(), utf8.length()) == 0;
    _ASSERTE(matchingBytes == true);
    Check(matchingBytes == true, "UTF-8 encoding");
}


void TestUnicodeConversions()
{
    std::cout << "*** Test Unicode UTF-16/UTF-8 std::wstring/string Conversion Functions *** \n"
        <<       "    ================================================================== \n"
        <<       "    by Giovanni Dicanio \n\n";

    TestEmptyStrings();
    TestStringsWithJapaneseKanji();
    TestStringLengths();
}


int main()
{
    // Run the tests
    TestUnicodeConversions();
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

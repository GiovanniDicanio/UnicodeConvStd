#ifndef GIOVANNI_DICANIO_UNICODECONVSTD_HPP_INCLUDED
#define GIOVANNI_DICANIO_UNICODECONVSTD_HPP_INCLUDED


////////////////////////////////////////////////////////////////////////////////
//
//            *** Unicode UTF-16/UTF-8 Conversion Functions ***
//                    with std::wstring and std::string
//
//                  Copyright (C) by Giovanni Dicanio
//                    <giovanni.dicanio AT gmail.com>
//
//
// Last Update: 2025, October 27th
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
//
// This is a header-only C++ library that implements a couple of functions
// to simply and conveniently convert Unicode text between UTF-16 and UTF-8.
//
// std::wstring is used to store UTF-16-encoded text.
// std::string is used to store UTF-8-encoded text.
//
// Input arguments use the respective string views:
//
// std::wstring_view is used for input UTF-16-encoded text.
// std::string_view is used for input UTF-8-encoded text.
//
// The exported functions are:
//
//      * Convert from UTF-16 to UTF-8:
//        std::string Utf8FromUtf16(std::wstring_view utf16)
//
//      * Convert from UTF-8 to UTF-16:
//        std::wstring Utf16FromUtf8(std::string_view utf8)
//
// These functions live under the UnicodeConvStd namespace.
//
// This code compiles cleanly at warning level 4 (/W4)
// on both 32-bit and 64-bit builds with Visual Studio 2019 in C++17 mode.
//
//------------------------------------------------------------------------------
//
// The MIT License(MIT)
//
// Copyright(c) 2016-2025 by Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//------------------------------------------------------------------------------


//==============================================================================
//                              Includes
//==============================================================================

#include <windows.h>    // Win32 Platform SDK

#include <crtdbg.h>     // _ASSERTE

#include <limits>       // std::numeric_limits
#include <stdexcept>    // std::runtime_error, std::overflow_error
#include <string>       // std::string, std::wstring
#include <string_view>  // std::string_view, std::wstring_view


//==============================================================================
//                              Public Interface
//==============================================================================

namespace UnicodeConvStd {


// Error during Unicode conversion
class UnicodeConversionException;


//------------------------------------------------------------------------------
// Convert from UTF-16 (std::wstring_view) to UTF-8 (std::string).
// Signal errors throwing UnicodeConversionException.
//------------------------------------------------------------------------------
[[nodiscard]] std::string Utf8FromUtf16(std::wstring_view utf16);

//------------------------------------------------------------------------------
// Convert from UTF-8 (std::string_view) to UTF-16 (std::wstring).
// Signal errors throwing UnicodeConversionException.
//------------------------------------------------------------------------------
[[nodiscard]] std::wstring Utf16FromUtf8(std::string_view utf8);



//==============================================================================
//                              Implementation
//==============================================================================


//------------------------------------------------------------------------------
// Represents an error during Unicode conversions
//------------------------------------------------------------------------------
class UnicodeConversionException
    : public std::runtime_error
{
public:

    enum class ConversionType
    {
        ConversionFromUtf16ToUtf8,
        ConversionFromUtf8ToUtf16
    };

    UnicodeConversionException(DWORD errorCode, ConversionType conversionType, const char* message)
        : std::runtime_error(message),
        m_errorCode(errorCode),
        m_conversionType(conversionType)
    {
    }

    UnicodeConversionException(DWORD errorCode, ConversionType conversionType, const std::string& message)
        : std::runtime_error(message),
        m_errorCode(errorCode),
        m_conversionType(conversionType)
    {
    }

    // Error code returned by Windows APIs like WideCharToMultiByte or MultiByteToWideChar
    [[nodiscard]] DWORD GetErrorCode() const noexcept
    {
        return m_errorCode;
    }

    [[nodiscard]] ConversionType GetConversionType() const noexcept
    {
        return m_conversionType;
    }

private:
    DWORD m_errorCode;
    ConversionType m_conversionType;
};


namespace detail
{

//------------------------------------------------------------------------------
// Helper function to safely convert a size_t value to an int.
// If the input size_t is too large, throws a std::overflow_error.
//------------------------------------------------------------------------------
inline [[nodiscard]] int SafeIntFromSizet(size_t s)
{
    using DestinationType = int;

    if (s > static_cast<size_t>((std::numeric_limits<DestinationType>::max)()))
    {
        throw std::overflow_error(
            "Input size_t value is too large: size_t-length doesn't fit into an int.");
    }

    return static_cast<DestinationType>(s);
}

} // namespace detail


inline std::string Utf8FromUtf16(std::wstring_view utf16)
{
    // Special case of empty input string
    if (utf16.empty())
    {
        // Empty input --> return empty output string
        return std::string{};
    }

    // Safely fail if an invalid UTF-16 character sequence is encountered
    constexpr DWORD kFlags = WC_ERR_INVALID_CHARS;

    const int utf16Length = detail::SafeIntFromSizet(utf16.length());

    // Get the length, in chars, of the resulting UTF-8 string
    const int utf8Length = ::WideCharToMultiByte(
        CP_UTF8,            // convert to UTF-8
        kFlags,             // conversion flags
        utf16.data(),       // source UTF-16 string
        utf16Length,        // length of source UTF-16 string, in wchar_ts
        nullptr,            // unused - no conversion required in this step
        0,                  // request size of destination buffer, in chars
        nullptr, nullptr    // unused
    );
    if (utf8Length == 0)
    {
        // Conversion error: capture error code and throw
        const DWORD errorCode = ::GetLastError();
        throw UnicodeConversionException(
            errorCode,
            UnicodeConversionException::ConversionType::ConversionFromUtf16ToUtf8,
            "Can't get result UTF-8 string length (WideCharToMultiByte failed).");
    }

    // Make room in the destination string for the converted bits
    std::string utf8(utf8Length, '\0');
    char* utf8Buffer = utf8.data();
    _ASSERTE(utf8Buffer != nullptr);

    // Do the actual conversion from UTF-16 to UTF-8
    int result = ::WideCharToMultiByte(
        CP_UTF8,            // convert to UTF-8
        kFlags,             // conversion flags
        utf16.data(),       // source UTF-16 string
        utf16Length,        // length of source UTF-16 string, in wchar_ts
        utf8Buffer,         // pointer to destination buffer
        utf8Length,         // size of destination buffer, in chars
        nullptr, nullptr    // unused
    );
    if (result == 0)
    {
        // Conversion error: capture error code and throw
        const DWORD errorCode = ::GetLastError();
        throw UnicodeConversionException(
            errorCode,
            UnicodeConversionException::ConversionType::ConversionFromUtf16ToUtf8,
            "Can't convert from UTF-16 to UTF-8 string (WideCharToMultiByte failed).");
    }

    return utf8;
}


inline std::wstring Utf16FromUtf8(std::string_view utf8)
{
    // Special case of empty input string
    if (utf8.empty())
    {
        // Empty input --> return empty output string
        return std::wstring{};
    }

    // Safely fail if an invalid UTF-8 character sequence is encountered
    constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

    const int utf8Length = detail::SafeIntFromSizet(utf8.length());

    // Get the size of the destination UTF-16 string
    const int utf16Length = ::MultiByteToWideChar(
        CP_UTF8,       // source string is in UTF-8
        kFlags,        // conversion flags
        utf8.data(),   // source UTF-8 string pointer
        utf8Length,    // length of the source UTF-8 string, in chars
        nullptr,       // unused - no conversion done in this step
        0              // request size of destination buffer, in wchar_ts
    );
    if (utf16Length == 0)
    {
        // Conversion error: capture error code and throw
        const DWORD errorCode = ::GetLastError();
        throw UnicodeConversionException(
            errorCode,
            UnicodeConversionException::ConversionType::ConversionFromUtf8ToUtf16,
            "Can't get result UTF-16 string length (MultiByteToWideChar failed).");
    }

    // Make room in the destination string for the converted bits
    std::wstring utf16(utf16Length, L'\0');
    wchar_t* utf16Buffer = utf16.data();
    _ASSERTE(utf16Buffer != nullptr);

    // Do the actual conversion from UTF-8 to UTF-16
    int result = ::MultiByteToWideChar(
        CP_UTF8,       // source string is in UTF-8
        kFlags,        // conversion flags
        utf8.data(),   // source UTF-8 string pointer
        utf8Length,    // length of source UTF-8 string, in chars
        utf16Buffer,   // pointer to destination buffer
        utf16Length    // size of destination buffer, in wchar_ts
    );
    if (result == 0)
    {
        // Conversion error: capture error code and throw
        const DWORD errorCode = ::GetLastError();
        throw UnicodeConversionException(
            errorCode,
            UnicodeConversionException::ConversionType::ConversionFromUtf8ToUtf16,
            "Can't convert from UTF-8 to UTF-16 string (MultiByteToWideChar failed).");
    }

    return utf16;
}


} // namespace UnicodeConvStd


#endif // GIOVANNI_DICANIO_UNICODECONVSTD_HPP_INCLUDED


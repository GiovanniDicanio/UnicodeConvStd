# UnicodeConvStd
Convenient easy-to-use and efficient helper functions to convert between Unicode UTF-16 and UTF-8 
using std::wstring and std::string

---

This project contains a header-only C++ file that implements a couple of functions
to simply, efficiently, and conveniently convert Unicode text between UTF-16 and UTF-8, 
using C++ Standard Library's `std::wstring` and `std::string` classes and direct Win32 API calls.

`std::wstring` is used to store **UTF-16**-encoded text.
`std::string` is used to store **UTF-8**-encoded text.

The exported functions are:

```cpp
    // Convert from UTF-16 to UTF-8
    std::string ToUtf8(std::wstring const& utf16)
    
    // Convert from UTF-8 to UTF-16
    std::wstring ToUtf16(std::string const& utf8)
```

These functions live under the `UnicodeConvStd` namespace.

This code compiles cleanly at warning level 4 (`/W4`)
on both 32-bit and 64-bit builds, with Visual Studio 2019 in C++17 mode.

Just `#include` [**`"UnicodeConvStd.hpp"`**](UnicodeConvStd/UnicodeConvStd.hpp) in your projects, 
and enjoy!

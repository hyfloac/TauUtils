#include "Console.hpp"
#include "allocator/PageAllocator.hpp"
#include "String.hpp"

TAU_LIB Console::CreatedState Console::IsCreated = NotCreated;
TAU_LIB bool Console::IsInitialized = false;

#define ETB_PAGE_COUNT (64)

class EncodingTranslationBuffer final
{
    DELETE_CM(EncodingTranslationBuffer);
public:
    EncodingTranslationBuffer() noexcept
        : m_Buffer(PageAllocator::Alloc(ETB_PAGE_COUNT))
    { }

    ~EncodingTranslationBuffer() noexcept
    { PageAllocator::Free(m_Buffer); }

    [[nodiscard]] void* Get() const noexcept { return m_Buffer; }

    template<typename T>
    [[nodiscard]] T* GetAs() const noexcept { return reinterpret_cast<T*>(m_Buffer); }

    [[nodiscard]] uSys GetSize() const noexcept { return ETB_PAGE_COUNT * PageAllocator::PageSize(); }

    template<typename T>
    [[nodiscard]] uSys GetSizeAs() const noexcept { return (ETB_PAGE_COUNT * PageAllocator::PageSize()) / sizeof(T); }
private:
    void* m_Buffer;
};

namespace tau::console::internal {

thread_local EncodingTranslationBuffer ETB;

}

#ifdef _WIN32
#include <Windows.h>

namespace tau::console::internal {

UINT OriginalConsoleInputCP = 0;
UINT OriginalConsoleOutputCP = 0;

HANDLE OriginalConsoleInputHandle = nullptr;
HANDLE OriginalConsoleOutputHandle = nullptr;
HANDLE OriginalConsoleErrorHandle = nullptr;

DWORD OriginalConsoleInputMode = 0;
DWORD OriginalConsoleOutputMode = 0;
DWORD OriginalConsoleErrorMode = 0;

}

void Console::Create() noexcept
{
    IsCreated = AllocConsole() ? Created : AlreadyExists;
}

void Console::ForceClose() noexcept
{
    FreeConsole();
}

#ifndef CP_UTF16LE
  #define CP_UTF16LE (1200)
#endif

#define CONSOLE_IN_DEFAULT_ARGS \
   (ENABLE_PROCESSED_INPUT | \
    ENABLE_LINE_INPUT | \
    ENABLE_ECHO_INPUT | \
    ENABLE_INSERT_MODE | \
    ENABLE_QUICK_EDIT_MODE)

#define CONSOLE_OUT_DEFAULT_ARGS \
   (ENABLE_PROCESSED_OUTPUT | \
    ENABLE_WRAP_AT_EOL_OUTPUT | \
    ENABLE_VIRTUAL_TERMINAL_PROCESSING)

void Console::ReInit() noexcept
{
    using namespace tau::console::internal;

    OriginalConsoleInputCP = GetConsoleCP();
    OriginalConsoleOutputCP = GetConsoleOutputCP();

    OriginalConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);
    OriginalConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    OriginalConsoleErrorHandle = GetStdHandle(STD_ERROR_HANDLE);

    GetConsoleMode(OriginalConsoleInputHandle, &OriginalConsoleInputMode);
    GetConsoleMode(OriginalConsoleOutputHandle, &OriginalConsoleOutputMode);
    GetConsoleMode(OriginalConsoleErrorHandle, &OriginalConsoleErrorMode);

    SetConsoleCP(CP_UTF16LE);
    SetConsoleOutputCP(CP_UTF16LE);

    SetConsoleMode(OriginalConsoleInputHandle, CONSOLE_IN_DEFAULT_ARGS);
    SetConsoleMode(OriginalConsoleOutputHandle, CONSOLE_OUT_DEFAULT_ARGS);
    SetConsoleMode(OriginalConsoleErrorHandle, CONSOLE_OUT_DEFAULT_ARGS);
}

void Console::Reset() noexcept
{
    using namespace tau::console::internal;

    SetConsoleCP(OriginalConsoleInputCP);
    SetConsoleCP(OriginalConsoleOutputCP);

    SetStdHandle(STD_INPUT_HANDLE, OriginalConsoleInputHandle);
    SetStdHandle(STD_OUTPUT_HANDLE, OriginalConsoleOutputHandle);
    SetStdHandle(STD_ERROR_HANDLE, OriginalConsoleErrorHandle);

    SetConsoleMode(OriginalConsoleInputHandle, OriginalConsoleInputMode);
    SetConsoleMode(OriginalConsoleOutputHandle, OriginalConsoleOutputMode);
    SetConsoleMode(OriginalConsoleErrorHandle, OriginalConsoleErrorMode);
}

u32 Console::Write(const char c) noexcept
{
    return Write(static_cast<wchar_t>(c));
}

u32 Console::Write(const wchar_t c) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    WriteConsoleW(OriginalConsoleOutputHandle, &c, 1, &numberOfCharsWritten, nullptr);
    return numberOfCharsWritten;
}

u32 Console::Write(const c8 c) noexcept
{
    return Write(static_cast<c16>(c));
}

u32 Console::Write(const c16 c) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    WriteConsoleW(OriginalConsoleOutputHandle, &c, 1, &numberOfCharsWritten, nullptr);
    return numberOfCharsWritten;
}

u32 Console::Write(const c32 c) noexcept
{
    using namespace tau::console::internal;

    c16 buf[2];
    iSys codeUnit = 0;
    const uSys wideLength = ::tau::string::utf16::EncodeCodePoint(c, buf, codeUnit, 2);

    DWORD numberOfCharsWritten;
    WriteConsoleW(OriginalConsoleOutputHandle, buf, static_cast<DWORD>(wideLength - 1), &numberOfCharsWritten, nullptr);
    return numberOfCharsWritten;
}

u32 Console::Write(const char* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;

    const uSys wideLength = MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(length), ETB.GetAs<wchar_t>(), static_cast<int>(ETB.GetSizeAs<wchar_t>()));
    return Write(ETB.GetAs<wchar_t>(), wideLength);
}

u32 Console::Write(const wchar_t* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    if(length > 0 && str[0] == 0xFEFF)
    {
        WriteConsoleW(OriginalConsoleOutputHandle, str + 1, static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else if(length > 0 && str[0] == 0xFFFE)
    {
        constexpr wchar_t errorMessage[] = L"[[Attempted to print Big Endian wchar_t string.]]\n";
        WriteConsoleW(OriginalConsoleErrorHandle, errorMessage, static_cast<DWORD>(cexpr::strlen(errorMessage)), &numberOfCharsWritten, nullptr);
    }
    else
    {
        WriteConsoleW(OriginalConsoleOutputHandle, str, static_cast<DWORD>(length), &numberOfCharsWritten, nullptr);
    }
    return numberOfCharsWritten;
}

u32 Console::Write(const c8* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;

    const uSys wideLength = tau::string::utf8_16::Transform(str, ETB.GetAs<c16>(), static_cast<iSys>(length), static_cast<iSys>(ETB.GetSizeAs<c16>()));
    return Write(ETB.GetAs<c16>(), wideLength);
}

u32 Console::Write(const c16* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    if(length > 0 && str[0] == 0xFEFF)
    {
        WriteConsoleW(OriginalConsoleOutputHandle, str + 1, static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else if(length > 0 && str[0] == 0xFFFE)
    {
        constexpr wchar_t errorMessage[] = L"[[Attempted to print Big Endian UTF16 string.]]\n";
        WriteConsoleW(OriginalConsoleErrorHandle, errorMessage, static_cast<DWORD>(cexpr::strlen(errorMessage)), &numberOfCharsWritten, nullptr);
    }
    else
    {
        WriteConsoleW(OriginalConsoleOutputHandle, str, static_cast<DWORD>(length), &numberOfCharsWritten, nullptr);
    }
    return numberOfCharsWritten;
}

u32 Console::Write(const c32* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;
    
    const uSys wideLength = tau::string::utf16::Transform(str, ETB.GetAs<c16>(), static_cast<iSys>(length), static_cast<iSys>(ETB.GetSizeAs<c16>()));
    return Write(ETB.GetAs<c16>(), wideLength);
}

bool Console::ReadLine(char* buffer, uSys maxLength, uSys* lineLengthCodePoints)
{
    using namespace tau::console::internal;

    return false;
}

bool Console::ReadLine(wchar_t* buffer, uSys maxLength, uSys* lineLengthCodePoints)
{
    using namespace tau::console::internal;

    return false;
}

bool Console::ReadLine(c8* buffer, uSys maxLength, uSys* lineLengthCodePoints)
{
    using namespace tau::console::internal;

    return false;
}

bool Console::ReadLine(c16* buffer, uSys maxLength, uSys* lineLengthCodePoints)
{
    using namespace tau::console::internal;

    return false;
}

bool Console::ReadLine(c32* buffer, uSys maxLength, uSys* lineLengthCodePoints)
{
    using namespace tau::console::internal;

    return false;
}

#endif

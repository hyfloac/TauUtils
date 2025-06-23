#include "Console.hpp"
#include "allocator/PageAllocator.hpp"
#include "String.hpp"

TAU_UTILS_LIB Console::CreatedState Console::IsCreated = NotCreated;
TAU_UTILS_LIB bool Console::IsInitialized = false;

class EncodingTransformationBuffer final
{
    DELETE_CM(EncodingTransformationBuffer);
public:
    static constexpr uSys TotalPageCount = 64;
public:
    explicit EncodingTransformationBuffer(const uSys totalPageCount = TotalPageCount) noexcept
        : m_TotalPageCount(totalPageCount)
        , m_Buffer(PageAllocator::Alloc(m_TotalPageCount))
    { }

    ~EncodingTransformationBuffer() noexcept
    { PageAllocator::Free(m_Buffer, TotalPageCount); }

    [[nodiscard]] void* Get() const noexcept { return m_Buffer; }

    template<typename T>
    [[nodiscard]] T* GetAs() const noexcept { return static_cast<T*>(m_Buffer); }

    [[nodiscard]] uSys GetSize() const noexcept { return m_TotalPageCount * PageAllocator::PageSize(); }

    template<typename T>
    [[nodiscard]] uSys GetSizeAs() const noexcept { return GetSize() / sizeof(T); }
private:
    uSys m_TotalPageCount;
    void* m_Buffer;
};

namespace tau::console::internal {

thread_local EncodingTransformationBuffer ETB;

}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
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
    (void) FreeConsole();
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

    (void) GetConsoleMode(OriginalConsoleInputHandle, &OriginalConsoleInputMode);
    (void) GetConsoleMode(OriginalConsoleOutputHandle, &OriginalConsoleOutputMode);
    (void) GetConsoleMode(OriginalConsoleErrorHandle, &OriginalConsoleErrorMode);

    (void) SetConsoleCP(CP_UTF16LE);
    (void) SetConsoleOutputCP(CP_UTF16LE);

    (void) SetConsoleMode(OriginalConsoleInputHandle, CONSOLE_IN_DEFAULT_ARGS);
    (void) SetConsoleMode(OriginalConsoleOutputHandle, CONSOLE_OUT_DEFAULT_ARGS);
    (void) SetConsoleMode(OriginalConsoleErrorHandle, CONSOLE_OUT_DEFAULT_ARGS);
}

void Console::Reset() noexcept
{
    using namespace tau::console::internal;

    (void) SetConsoleCP(OriginalConsoleInputCP);
    (void) SetConsoleCP(OriginalConsoleOutputCP);

    (void) SetStdHandle(STD_INPUT_HANDLE, OriginalConsoleInputHandle);
    (void) SetStdHandle(STD_OUTPUT_HANDLE, OriginalConsoleOutputHandle);
    (void) SetStdHandle(STD_ERROR_HANDLE, OriginalConsoleErrorHandle);

    (void) SetConsoleMode(OriginalConsoleInputHandle, OriginalConsoleInputMode);
    (void) SetConsoleMode(OriginalConsoleOutputHandle, OriginalConsoleOutputMode);
    (void) SetConsoleMode(OriginalConsoleErrorHandle, OriginalConsoleErrorMode);
}

u32 Console::Write(const char c) noexcept
{
    return Write(static_cast<wchar_t>(c));
}

u32 Console::Write(const wchar_t c) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    (void) WriteConsoleW(OriginalConsoleOutputHandle, &c, 1, &numberOfCharsWritten, nullptr);
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
    (void) WriteConsoleW(OriginalConsoleOutputHandle, &c, 1, &numberOfCharsWritten, nullptr);
    return numberOfCharsWritten;
}

u32 Console::Write(const c32 c) noexcept
{
    using namespace tau::console::internal;

    c16 buf[2];
    iSys codeUnit = 0;
    const uSys wideLength = ::tau::string::utf16::EncodeCodePoint(c, buf, codeUnit, 2);

    DWORD numberOfCharsWritten;
    (void) WriteConsoleW(OriginalConsoleOutputHandle, buf, static_cast<DWORD>(wideLength), &numberOfCharsWritten, nullptr);
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
        (void) WriteConsoleW(OriginalConsoleOutputHandle, str + 1, static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else if(length > 0 && str[0] == 0xFFFE)
    {
        ::tau::string::utf16::FlipEndian(str + 1, ETB.GetAs<wchar_t>(), static_cast<iSys>(length - 1));
        (void) WriteConsoleW(OriginalConsoleErrorHandle, ETB.GetAs<wchar_t>(), static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else
    {
        (void) WriteConsoleW(OriginalConsoleOutputHandle, str, static_cast<DWORD>(length), &numberOfCharsWritten, nullptr);
    }
    return numberOfCharsWritten;
}

u32 Console::Write(const c8* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;

    const uSys wideLength = ::tau::string::utf8_16::Transform(str, ETB.GetAs<c16>(), static_cast<iSys>(length), static_cast<iSys>(ETB.GetSizeAs<c16>()), false, true);
    return Write(ETB.GetAs<c16>(), wideLength);
}

u32 Console::Write(const c16* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;
    
    DWORD numberOfCharsWritten;
    if(length > 0 && str[0] == 0xFEFF)
    {
        (void) WriteConsoleW(OriginalConsoleOutputHandle, str + 1, static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else if(length > 0 && str[0] == 0xFFFE)
    {
        ::tau::string::utf16::FlipEndian(str + 1, ETB.GetAs<c16>(), static_cast<iSys>(length - 1));
        (void) WriteConsoleW(OriginalConsoleErrorHandle, ETB.GetAs<c16>(), static_cast<DWORD>(length - 1), &numberOfCharsWritten, nullptr);
    }
    else
    {
        (void) WriteConsoleW(OriginalConsoleOutputHandle, str, static_cast<DWORD>(length), &numberOfCharsWritten, nullptr);
    }
    return numberOfCharsWritten;
}

u32 Console::Write(const c32* const str, const uSys length) noexcept
{
    using namespace tau::console::internal;
    
    const uSys wideLength = ::tau::string::utf16::Transform(str, ETB.GetAs<c16>(), static_cast<iSys>(length), static_cast<iSys>(ETB.GetSizeAs<c16>()), false, true);
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

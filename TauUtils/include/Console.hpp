#pragma once

#include "NumTypes.hpp"

class Console final
{
public:
    static void Create() noexcept;
    
    static void ForceClose() noexcept;

    static void Close() noexcept
    {
        if(IsCreated == Created)
        {
            Close();
        }
    }

    static void Init() noexcept
    {
        if(!IsInitialized)
        {
            IsInitialized = true;
            ReInit();
        }
    }

    static void ReInit() noexcept;
    static void Reset() noexcept;

    static u32 Write(char c) noexcept;
    static u32 Write(wchar_t c) noexcept;
    static u32 Write(c8 c) noexcept;
    static u32 Write(c16 c) noexcept;
    static u32 Write(c32 c) noexcept;

    static u32 Write(const char* str, uSys length) noexcept;
    static u32 Write(const wchar_t* str, uSys length) noexcept;
    static u32 Write(const c8* str, uSys length) noexcept;
    static u32 Write(const c16* str, uSys length) noexcept;
    static u32 Write(const c32* str, uSys length) noexcept;

    static c32 ReadChar() noexcept;
    static c32 ReadCharInvisible() noexcept;

    static iSys ReadString();
private:
    enum CreatedState
    {
        NotCreated,
        Created,
        AlreadyExists
    };

    static CreatedState IsCreated;
    static bool IsInitialized;
};

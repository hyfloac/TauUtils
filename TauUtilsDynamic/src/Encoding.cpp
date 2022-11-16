#include "Encoding.hpp"
#include <unordered_map>

namespace tau::utils::internal {

::std::unordered_map<const char*, Ref<Encoding>> g_Encodings;

}

NullableRef<Encoding> Encoding::ForName(const char* const name) noexcept
{
    using tau::utils::internal::g_Encodings;

    if(g_Encodings.contains(name))
    {
        return g_Encodings.at(name);
    }

    return nullptr;
}

void Encoding::RegisterEncoding(const char* const name, const NullableRef<Encoding>& codec) noexcept
{
    tau::utils::internal::g_Encodings.insert_or_assign(name, codec);
}

void Encoding::RegisterEncoding(const char* const name, NullableRef<Encoding>&& codec) noexcept
{
    tau::utils::internal::g_Encodings.insert_or_assign(name, ::std::move(codec));
}

void Encoding::UnregisterEncoding(const char* const name)
{
    tau::utils::internal::g_Encodings.erase(name);
}

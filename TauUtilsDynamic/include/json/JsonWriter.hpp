#pragma once

#include "JsonTypes.hpp"
#include "../ToString.hpp"
#include "../Dragon4.hpp"

namespace tau::json {

class JsonWriter final
{
    DEFAULT_CONSTRUCT_PU(JsonWriter);
    DEFAULT_DESTRUCT(JsonWriter);
    DELETE_CM(JsonWriter);
public:
    void Begin(const StrongRef<JObject>& root) noexcept
    {
        Write(root);
    }

    void Begin(const StrongRef<JArray>& root) noexcept
    {
        Write(root);
    }

    void Reset() noexcept
    {
        m_Builder.reset();
    }

    [[nodiscard]] C8DynString ToString() const noexcept
    {
        return m_Builder.toString();
    }
private:
    void Write(const StrongRef<JString>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }

        m_Builder.Append(u8'\"');
        m_Builder.Append(root->String());
        m_Builder.Append(u8'\"');
    }

    void Write(const StrongRef<JInteger>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }

        c8 buffer[::tau::MaxCharCount<i64>::Value];
        ::tau::ItoA(root->I64(), buffer, ::tau::MaxCharCount<i64>::Value);

        m_Builder.Append(buffer);
    }

    void Write(const StrongRef<JReal>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }

        c8 buffer[256];
        (void) PrintFloat64(buffer, ::std::size(buffer), root->Double(), PrintFloatFormat::Positional, 17);
        m_Builder.Append(buffer);
    }

    void Write(const StrongRef<JObject>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }

        m_Builder.Append(u8'{');

        bool hadPrevious = false;

        for(const auto& pair : root->Iterable())
        {
            if(hadPrevious)
            {
                m_Builder.Append(u8',');
            }

            m_Builder.Append(u8'\"');
            m_Builder.Append(pair.first);
            m_Builder.Append(u8'\"');
            m_Builder.Append(u8':');

            switch(pair.second->Type())
            {
                case EJType::String: Write(pair.second->InternalGetWrapper(pair.second).AsString()); break;
                case EJType::Integer: Write(pair.second->InternalGetWrapper(pair.second).AsInteger()); break;
                case EJType::Real: Write(pair.second->InternalGetWrapper(pair.second).AsReal()); break;
                case EJType::Object: Write(pair.second->InternalGetWrapper(pair.second).AsObject()); break;
                case EJType::Array: Write(pair.second->InternalGetWrapper(pair.second).AsArray()); break;
                case EJType::Bool: Write(pair.second->InternalGetWrapper(pair.second).AsBool()); break;
                case EJType::Null: Write(pair.second->InternalGetWrapper(pair.second).AsNull()); break;
                default: break;
            }

            hadPrevious = true;
        }

        m_Builder.Append(u8'}');
    }

    void Write(const StrongRef<JArray>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }

        m_Builder.Append(u8'[');

        bool hadPrevious = false;

        for(const auto& value : root->Iterable())
        {
            if(hadPrevious)
            {
                m_Builder.Append(u8',');
            }

            switch(value->Type())
            {
                case EJType::String: Write(value->InternalGetWrapper(value).AsString()); break;
                case EJType::Integer: Write(value->InternalGetWrapper(value).AsInteger()); break;
                case EJType::Real: Write(value->InternalGetWrapper(value).AsReal()); break;
                case EJType::Object: Write(value->InternalGetWrapper(value).AsObject()); break;
                case EJType::Array: Write(value->InternalGetWrapper(value).AsArray()); break;
                case EJType::Bool: Write(value->InternalGetWrapper(value).AsBool()); break;
                case EJType::Null: Write(value->InternalGetWrapper(value).AsNull()); break;
                default: break;
            }

            hadPrevious = true;
        }

        m_Builder.Append(u8']');
    }

    void Write(const StrongRef<JBool>& root) noexcept
    {
        if(!root)
        {
            m_Builder.Append(u8"null");
            return;
        }
        
        if(root->Bool())
        {
            m_Builder.Append(u8"true");
        }
        else
        {
            m_Builder.Append(u8"false");
        }
    }

    void Write(const StrongRef<JNull>& root) noexcept
    {
        m_Builder.Append(u8"null");
    }
private:
    C8StringBuilder m_Builder;
};

}

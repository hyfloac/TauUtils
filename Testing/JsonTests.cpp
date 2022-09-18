#include <json/JsonWriter.hpp>
#include <ConPrinter.hpp>

static void JsonPrintTest() noexcept
{
    ::tau::json::JRoot jRoot = ::tau::json::JRoot::CreateObject();
    StrongRef<::tau::json::JObject> root = jRoot.Get().AsObject();

    root->Insert(root, C8DynString(u8"test"), C8DynString(u8"Hello, World!"));
    root->Insert(root, C8DynString(u8"int"), 42ull);
    auto subObject = root->InsertObject(root, C8DynString(u8"obj"));
    auto subSubArray = subObject->InsertArray(subObject, C8DynString(u8"arr"));
    subSubArray->Append(subSubArray, 3.14);
    subSubArray->Append(subSubArray, 2.718);

    ::tau::json::JsonWriter writer;
    writer.Begin(root);

    ConPrinter::PrintLn("{}", writer.ToString());
}

void JsonTests()
{
    JsonPrintTest();
}

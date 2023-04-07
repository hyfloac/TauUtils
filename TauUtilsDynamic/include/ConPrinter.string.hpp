#pragma once

#include "ConPrinter.hpp"
#include <string>

namespace ConPrinter {

static u32 Print(const ::std::string& t) noexcept { return Print(t.c_str()); }
static u32 Print(const ::std::wstring& t) noexcept { return Print(t.c_str()); }

}

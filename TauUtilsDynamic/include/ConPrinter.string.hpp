#pragma once

#include "ConPrinter.hpp"
#include <string>

namespace ConPrinter {

static i32 Print(const ::std::string& t) noexcept { return Print(t.c_str()); }
static i32 Print(const ::std::wstring& t) noexcept { return Print(t.c_str()); }

}

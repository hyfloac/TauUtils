/**
 * @file
 *
 * Heavily based on https://www.ryanjuckett.com/printing-floating-point-numbers-part-2-dragon4/
 */
#pragma once

/******************************************************************************
  Copyright (c) 2014 Ryan Juckett
  http://www.ryanjuckett.com/

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software. -- It has been modified.

  3. This notice may not be removed or altered from any source
     distribution.
******************************************************************************/


#include "NumTypes.hpp"

enum class CutoffMode
{
    Unique,        // As many digits as necessary to print a uniquely identifiable number.
    TotalLength,   // Up to cutoffNumber significant digits.
    FractionLength // Up to cutoffNumber significant digits past the decimal point.
};

enum class PrintFloatFormat
{
    Positional, // [-]ddddd.dddd
    Scientific  // [-]d.dddde[sign]ddd
};

uSys Dragon4(u64 mantissa, i32 exponent, u32 mantissaHighBitIndex, bool hasUnequalMargins, CutoffMode cutoffMode, u32 cutoffNumber, char* buffer, uSys bufferSize, i32* outExponent) noexcept;
uSys Dragon4(u64 mantissa, i32 exponent, u32 mantissaHighBitIndex, bool hasUnequalMargins, CutoffMode cutoffMode, u32 cutoffNumber, wchar_t* buffer, uSys bufferSize, i32* outExponent) noexcept;
uSys Dragon4(u64 mantissa, i32 exponent, u32 mantissaHighBitIndex, bool hasUnequalMargins, CutoffMode cutoffMode, u32 cutoffNumber, c8* buffer, uSys bufferSize, i32* outExponent) noexcept;
uSys Dragon4(u64 mantissa, i32 exponent, u32 mantissaHighBitIndex, bool hasUnequalMargins, CutoffMode cutoffMode, u32 cutoffNumber, c16* buffer, uSys bufferSize, i32* outExponent) noexcept;
uSys Dragon4(u64 mantissa, i32 exponent, u32 mantissaHighBitIndex, bool hasUnequalMargins, CutoffMode cutoffMode, u32 cutoffNumber, c32* buffer, uSys bufferSize, i32* outExponent) noexcept;

// If precision is negative, the minimum number of digits to represent a
// unique 32-bit floating point value is output. Otherwise,
// this is the number of digits to print past the decimal point.

uSys PrintFloat32(char* outBuffer, uSys bufferSize, f32 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat32(wchar_t* outBuffer, uSys bufferSize, f32 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat32(c8* outBuffer, uSys bufferSize, f32 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat32(c16* outBuffer, uSys bufferSize, f32 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat32(c32* outBuffer, uSys bufferSize, f32 value, PrintFloatFormat format, i32 precision) noexcept;

// If precision is negative, the minimum number of digits to represent a
// unique 32-bit floating point value is output. Otherwise,
// this is the number of digits to print past the decimal point.

uSys PrintFloat64(char* outBuffer, uSys bufferSize, f64 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat64(wchar_t* outBuffer, uSys bufferSize, f64 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat64(c8* outBuffer, uSys bufferSize, f64 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat64(c16* outBuffer, uSys bufferSize, f64 value, PrintFloatFormat format, i32 precision) noexcept;
uSys PrintFloat64(c32* outBuffer, uSys bufferSize, f64 value, PrintFloatFormat format, i32 precision) noexcept;

/* SourceMap-Qt
 * (C) Copyright 2014 HicknHack Software GmbH
 *
 * The original code can be found at:
 *     https://github.com/hicknhack-software/SourceMap-Qt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Base64VLQ.h"

#include <limits>
#include <assert.h>
#include <cstdint>
#include <vector>

namespace SourceMap {
namespace intern {
namespace Base64VLQ {
namespace {

// A single base 64 digit can contain 6 bits of data. For the base 64 variable
// length quantities we use in the source map spec, the first bit is the sign,
// the next four bits are the actual value, and the 6th bit is the
// continuation bit. The continuation bit tells us whether there are more
// digits in this value following this digit.
//
//   Continuation
//   |    Sign
//   |    |
//   V    V
//   101011
#define VLQ_BASE_SHIFT 5
#define VLQ_BASE (1 << VLQ_BASE_SHIFT)
#define VLQ_BASE_MASK (VLQ_BASE - 1)
#define VLQ_CONTINUATION_BIT VLQ_BASE

#define BASE64_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

string::value_type base64_encode(std::uint8_t digit) {
    static const auto intToCharStr = string{BASE64_CHARS};
    assert(digit < intToCharStr.size());
    return intToCharStr.at(digit);
}

#define INVALID_DIGIT std::numeric_limits<std::uint8_t>::max()
#define buildReverseMap(chars) \
([](){ \
    auto result = std::vector<std::uint8_t>(128, INVALID_DIGIT); \
    auto value = 0; \
    for (auto chr : chars) result[static_cast<int>(chr)] = value++; \
    return result; \
}()) \

int base64_decode(string::value_type chr) {
    static const auto charToValue = buildReverseMap(BASE64_CHARS);
    return charToValue.at(chr.unicode());
}

/**
 * Converts from a two-complement value to a value where the sign bit is
 * is placed in the least significant bit. For example, as decimals:
 *   1 becomes 2 (10 binary), -1 becomes 3 (11 binary)
 *   2 becomes 4 (100 binary), -2 becomes 5 (101 binary)
 */
int toVLQSigned(int aValue) {
    return aValue < 0
            ? ((-aValue) << 1) + 1
            : (aValue << 1) + 0;
}

/**
 * Converts to a two-complement value from a value where the sign bit is
 * is placed in the least significant bit. For example, as decimals:
 *   2 (10 binary) becomes 1, 3 (11 binary) becomes -1
 *   4 (100 binary) becomes 2, 5 (101 binary) becomes -2
 */
int fromVLQSigned(int aValue) {
    const auto isNegative = (aValue & 1) == 1;
    const auto shifted = aValue >> 1;
    return isNegative
            ? -shifted
            : shifted;
}

} // namespace

void encode(string_ref str, int value)
{
    auto vlq = toVLQSigned(value);
    do {
      auto digit = vlq & VLQ_BASE_MASK;
      vlq = vlq >> VLQ_BASE_SHIFT;
      if (vlq > 0) digit = digit | VLQ_CONTINUATION_BIT;
      str.get().append(base64_encode(digit));
    } while (vlq > 0);
}

bool decode(const_iterator_ref begin, const_iterator end, int_ref result)
{
    if (begin.get() == end) return false; // nothing to read
    const int digit = base64_decode(*begin.get());
    if (INVALID_DIGIT == digit) return false; // no valid digit
    ++begin.get();
    auto hasNext = (digit & VLQ_CONTINUATION_BIT) != 0;
    auto decoded = digit & VLQ_BASE_MASK;
    auto shift = 0;
    while (hasNext) {
        if (begin.get() == end) return false; // reached end (encoding error!)
        const auto digit = base64_decode(*begin.get());
        ++begin.get();
        hasNext = (digit & VLQ_CONTINUATION_BIT) != 0;
        decoded += (digit & VLQ_BASE_MASK) << (shift += VLQ_BASE_SHIFT);
    }
    result.get() = fromVLQSigned(decoded);
    return true;
}

int decode(const_iterator_ref begin, const_iterator end, int error)
{
    decode(begin, end, std::ref(error));
    return error;
}

} // namespace Base64VLQ
} // namespace intern
} // namespace SourceMap

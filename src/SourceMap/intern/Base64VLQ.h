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
#ifndef SOURCEMAP_INTERN_BASE64VLQ_H
#define SOURCEMAP_INTERN_BASE64VLQ_H

#include <QString>
#include <functional>

namespace SourceMap {
namespace intern {

/// Base 64 encoded variable length encoding for signed integer values
namespace Base64VLQ {

// TODO: this is an attempt to universalize string,
// but it misses access to the int value of a char (QChar has no int converters)
using string = QString;
using string_ref = std::reference_wrapper< string >;
using iterator = string::iterator;
using iterator_ref = std::reference_wrapper< iterator >;
using const_iterator = string::const_iterator;
using const_iterator_ref = std::reference_wrapper< const_iterator >;
using int_ref = std::reference_wrapper< int >;

/// stores the value into the string
void encode(string_ref str, int value);

/// @returns true, if result contains the decoded value otherwise result keeps it's initial value
bool decode(const_iterator_ref begin, const_iterator end, int_ref result);

/// @returns the decoded value otherwise error value
int decode(const_iterator_ref begin, const_iterator end, int error);

} // namespace Base64VLQ
} // namespace intern
} // namespace SourceMap

#endif // SOURCEMAP_INTERN_BASE64VLQ_H

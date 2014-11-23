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
#ifndef SOURCEMAP_EXTENSION_CALLER_H
#define SOURCEMAP_EXTENSION_CALLER_H

#include "SourceMap/Extension/Base.h"

#include "SourceMap/FilePosition.h"

#include <vector>

namespace SourceMap {

template< typename... ExtensionTypes >
struct Entry;

template< typename... ExtensionTypes >
struct Data;

struct Caller;
using CallerList = std::vector< Caller >;

using CallerStack = std::vector< FilePosition >;

// Visual Studio 2013 has no constexpr for variables
#define InvalidCallerIndex -1

/// encapsulates the index to a caller
struct CallerIndex
{
    CallerIndex() {}
    explicit CallerIndex(int value) : value(value) {}

    int value = InvalidCallerIndex;
};

/**
 * @brief Caller represents an entry in a caller hierarchy
 *
 * An instance with an invalid origin position is considered invalid
 */
struct Caller
{
    /// constructs an invalid caller
    Caller() {}

    /// constructs a valid Caller
    explicit Caller(const FilePosition& _original, CallerIndex _parentIndex = {})
        : original(_original)
        , parentIndex(_parentIndex)
    {}

    /// constructs a valid Caller
    explicit Caller(FilePosition&& _original, CallerIndex _parentIndex = {})
        : original(std::move(_original))
        , parentIndex(_parentIndex)
    {}

    inline bool isValid() const { return original.isValid(); }

    FilePosition original; ///< source position
    CallerIndex parentIndex; ///< index of the parent caller (-1 if none)
};

template< typename... ExtensionTypes >
CallerStack buildCallerStack(const Data< ExtensionTypes... > &data,
                             const Entry< ExtensionTypes... > *entry);

namespace Extension {

/// This implements the Extension interface for the Interpolation
struct Caller : Base
{
    using EntryData = CallerIndex; ///< one index per entry
    using MapData = CallerList; ///< list of all callers

    template< typename Mapping >
    static void jsonEncode(const Mapping&, std::reference_wrapper<RevisionThree>);

    template< typename Data >
    static bool jsonDecode(std::reference_wrapper<Data>, const RevisionThree&);
};

} // namespace Extension
} // namespace SourceMap

#include "Caller_impl.h"

#endif // SOURCEMAP_EXTENSION_CALLER_H

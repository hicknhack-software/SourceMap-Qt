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
#pragma once

#include "SourceMap/Position.h"
#include "SourceMap/FilePosition.h"
#include "SourceMap/Extensions.h"

#include "SourceMap/meta/tuple_index_of.h"

#include <QString>
#include <vector>

namespace SourceMap {

template< typename ...ExtensionTypes >
struct Entry;

template< typename ...ExtensionTypes >
using EntryList = std::vector< Entry< ExtensionTypes... > >;

/**
 * @brief represents the data for one mapping entry
 *
 * * maps a position of the original text to a position of the generated text
 * * supports any number of extensions
 *
 * this is a pure data container structure
 */
template< typename ...ExtensionTypes >
struct Entry
{
    using Extensions = SourceMap::Extensions< ExtensionTypes... >;
    using ExtensionData = typename Extensions::EntryData;

    /// constructs an invalid entry
    Entry(){}

    /// constructs an entry without an original location
    explicit Entry(Position generated,
                   ExtensionData extensionData = {})
        : Entry(std::move(generated), {}, {}, std::move(extensionData))
    {}

    /// construct a mapping entry without a symbol name
    Entry(Position generated,
          FilePosition original,
          ExtensionData extensionData = {})
        : Entry(std::move(generated), std::move(original), {}, std::move(extensionData))
    {}

    /// construct a mapping entry with a symbol name
    Entry(Position generated,
          FilePosition original,
          QString name,
          ExtensionData extensionData = {})
        : generated(std::move(generated))
        , original(std::move(original))
        , name(std::move(name))
        , extensionData(std::move(extensionData))
    {}

    /// @returns true if the entry is useful
    inline auto isValid() const -> bool { return generated.isValid(); }

    Position generated; ///< text start position in the generated file
    FilePosition original; ///< text start position in the original/source file
    QString name; ///< name of the symbol (used for debuggers)
    ExtensionData extensionData; ///< addtional data for all extensions
};

template< typename ExtensionType, typename ...ExtensionTypes >
inline auto get(const Entry<ExtensionTypes...>& entry) -> const typename ExtensionType::EntryData&
{
    const int index = meta::tuple_index_of<
            typename ExtensionType::EntryData,
            typename Entry<ExtensionTypes...>::ExtensionData
            >::value;
    return std::get< index >(entry.extensionData);
}

template< typename ExtensionType, typename ...ExtensionTypes >
inline auto get(Entry<ExtensionTypes...>& entry) -> typename ExtensionType::EntryData&
{
    const int index = meta::tuple_index_of<
            typename ExtensionType::EntryData,
            typename Entry<ExtensionTypes...>::ExtensionData
            >::value;
    return std::get< index >(entry.extensionData);
}

} // namespace SourceMap


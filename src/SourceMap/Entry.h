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
#ifndef SOURCEMAP_ENTRY_H
#define SOURCEMAP_ENTRY_H

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
 * @brief maps between begining of original and beginning of generated text
 *
 * supports any number of extensions
 */
template< typename ...ExtensionTypes >
struct Entry
{
    using Extensions = SourceMap::Extensions< ExtensionTypes... >;
    using ExtensionData = typename Extensions::EntryData;

    /// constructs an invalid entry
    Entry(){}

    /// constructs an entry without an original location (no interpolation allowed)
    explicit Entry(Position&& _generated)
        : Entry(std::move(_generated), {}, {}, {})
    {}

    /// construct a mapping entry without a symbol name
    Entry(Position&& _generated, const FilePosition& _original, ExtensionData&& _extensionData = {})
        : Entry(std::move(_generated), _original, {}, std::move(_extensionData))
    {}
    Entry(Position&& _generated, FilePosition&& _original, ExtensionData&& _extensionData = {})
        : Entry(std::move(_generated), std::move(_original), {}, std::move(_extensionData))
    {}

    /// construct a mapping entry with a symbol name
    Entry(Position&& _generated, const FilePosition& _original, QString&& _name, ExtensionData&& _extensionData = {})
        : generated(std::move(_generated))
        , original(_original)
        , name(std::move(_name))
        , extensionData(std::move(_extensionData))
    {}
    Entry(Position&& _generated, FilePosition&& _original, QString&& _name, ExtensionData&& _extensionData = {})
        : generated(std::move(_generated))
        , original(std::move(_original))
        , name(std::move(_name))
        , extensionData(std::move(_extensionData))
    {}

    /// @returns true if the entry is useful
    inline bool isValid() const { return generated.isValid(); }

    Position generated; ///< text start position in the generated file
    FilePosition original; ///< text start position in the original/source file
    QString name; ///< name of the symbol (used for debuggers)
    ExtensionData extensionData; ///< addtional data for all extensions
};

template< typename ExtensionType, typename ...ExtensionTypes >
inline const typename ExtensionType::EntryData&
get(const Entry<ExtensionTypes...>& entry)
{
    const int index = meta::tuple_index_of< typename ExtensionType::EntryData, typename Entry<ExtensionTypes...>::ExtensionData>::value;
    return std::get< index >(entry.extensionData);
}

template< typename ExtensionType, typename ...ExtensionTypes >
inline typename ExtensionType::EntryData &
get(Entry<ExtensionTypes...>& entry)
{
    const int index = meta::tuple_index_of< typename ExtensionType::EntryData, typename Entry<ExtensionTypes...>::ExtensionData>::value;
    return std::get< index >(entry.extensionData);
}

} // namespace SourceMap

#endif // SOURCEMAP_ENTRY_H

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

#include "SourceMap/Entry.h"
#include "SourceMap/Extensions.h"

#include "SourceMap/meta/tuple_index_of.h"

namespace SourceMap {

/**
 * @brief encapsulates all input data of a source map
 *
 * * each entry marks the mapping of starting positions
 * * supports any number of extensions
 */
template< typename... ExtensionTypes >
struct Data
{
    using Entry = SourceMap::Entry< ExtensionTypes... >;
    using EntryList = SourceMap::EntryList< ExtensionTypes... >;
    using Extensions = SourceMap::Extensions< ExtensionTypes... >;
    using ExtensionData = typename Extensions::MapData;
    using IncludeList = std::vector< QString >;

    /**
     * @brief constructor
     *
     * uses compiler optimisations
     */
    Data(EntryList entries = {}, ExtensionData extensionData = {})
        : extensionData(std::move(extensionData))
        , m_entries(std::move(entries))
    {}

    ExtensionData extensionData;

    inline const EntryList &entries() const { return m_entries; }
    inline EntryList &entries() { return m_entries; }
    inline IncludeList includes() const { return m_includes; }

    bool addEntry(const Entry &entry);
    void addInclude(const QString &filename);

private:
    EntryList m_entries;
    IncludeList m_includes;
};

template< typename ExtensionType, typename... ExtensionTypes >
inline const typename ExtensionType::MapData&
get(const Data<ExtensionTypes...>& data)
{
    const int index = meta::tuple_index_of<
            typename ExtensionType::MapData,
            typename Data<ExtensionTypes...>::ExtensionData
            >::value;
    return std::get< index >(data.extensionData);
}

template< typename ExtensionType, typename ...ExtensionTypes >
inline typename ExtensionType::MapData &
get(Data<ExtensionTypes...>& data)
{
    const int index = meta::tuple_index_of<
            typename ExtensionType::MapData,
            typename Data<ExtensionTypes...>::ExtensionData
            >::value;
    return std::get< index >(data.extensionData);
}

} // namespace SourceMap

#include "Data_impl.h"


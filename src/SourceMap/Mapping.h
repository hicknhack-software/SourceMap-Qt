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
#ifndef SOURCEMAP_MAPPING_H
#define SOURCEMAP_MAPPING_H

#include "SourceMap/Entry.h"
#include "SourceMap/Data.h"

#include <QStringList>

#include <memory>

namespace SourceMap {

/**
 * @brief implements a mapping from generated positions to original positions
 */
template< typename... ExtensionTypes >
class Mapping
{
public:
    using Entry = SourceMap::Entry< ExtensionTypes... >;
    using EntryList = SourceMap::EntryList< ExtensionTypes... >;
    using Data = SourceMap::Data< ExtensionTypes... >;
    using Extensions = SourceMap::Extensions< ExtensionTypes... >;

public:
    explicit Mapping(const Data& data);
    explicit Mapping(const Data&& data);

    /// @returns raw data
    const Data& data() const;
    operator Data&() const { return data(); }

    /// @returns sorted Entries
    const EntryList& entriesSortedToGeneratedPosition() const;

    /// @returns List of all files involved in the generation
    /// List has no duplicates
    const QStringList &originalNames() const;

    /// @returns nullptr for an invalid position
    /// Pointer to the mapping entry of the generated position
    const Entry *findEntryByGenerated(const Position &position) const;

private:
    class Private;
    const std::shared_ptr<Private> m_private;
};

} // namespace SourceMap

#include "Mapping_impl.h"

#endif // SOURCEMAP_MAPPING_H

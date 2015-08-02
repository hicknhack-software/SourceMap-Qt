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
#include "SourceMap/Data.h"

#include <QStringList>

#include <memory>

namespace SourceMap {

/**
 * @brief implements a search interface
 *
 * * find the original entry for a position in the generated text
 * * list all original files involved
 *
 * This class encapsulates a state and generates search indices
 * It's cheap to copy and uses an internal shared_pointer pimpl
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
    inline Mapping() {}
    explicit Mapping(Data data);

    /// @returns raw data
    const Data& data() const;

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
    std::shared_ptr<Private> m_private;
};

} // namespace SourceMap

#include "Mapping_impl.h"


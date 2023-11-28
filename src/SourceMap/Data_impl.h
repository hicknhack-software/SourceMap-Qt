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

#include "SourceMap/Data.h"

namespace SourceMap {

template< typename... ExtensionTypes >
auto Data< ExtensionTypes... >::addEntry(const typename Data::Entry &entry) -> bool
{
    if (m_entries.empty()) {
        m_entries.push_back(entry);
        return false;
    }

    const Data::Entry &lastEntry = m_entries.back();
    const Position &lastPosition = lastEntry.generated;
    const Position &entryPosition = entry.generated;

    const int compare = comparePositions(lastPosition, entryPosition);
    bool override = false;
    if (compare > 0) {
        m_entries.push_back(entry);
    }
    else if (compare == 0) {
        m_entries[m_entries.size() - 1] = entry;
        override = true;
    }

    return override;
}

} // namespace SourceMap

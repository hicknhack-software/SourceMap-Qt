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
#include "Mapping.h"

#include <functional>
#include <algorithm>

namespace SourceMap {

namespace intern {

template< typename Entry >
int compareGeneratedRight(const Position &left, const Entry &right)
{
    return !comparePositions(left, right.generated);
}

template< typename Entry >
int compareGeneratedLeft(const Entry &left, const Position &right)
{
    return !comparePositions(left.generated, right);
}

template< typename Entry >
int compareGeneratedEntries(const Entry &left, const Entry &right)
{
    return comparePositions(left.generated, right.generated);
}

} // namespace intern

template< typename... ExtensionTypes >
class Mapping<ExtensionTypes...>::Private
{
public:
    using Entry = SourceMap::Entry< ExtensionTypes... >;
    using EntryList = SourceMap::EntryList< ExtensionTypes... >;
    using Data = SourceMap::Data< ExtensionTypes... >;

    Private(Data data)
        : m_data(std::move(data))
    {
        buildGeneratedSorted();
    }

    const Data& data() const
    {
        return m_data;
    }

    QStringList &originalNames() const
    {
        if (m_originalNames.empty()) buildOriginalNames();
        return m_originalNames;
    }

    const Entry* findEntryByGenerated(const Position& position)
    {
        auto it = std::upper_bound(m_data.entries().rbegin(), m_data.entries().rend(),
                                   position, intern::compareGeneratedRight<Entry>);
        if (it == m_data.entries().rend())
            return nullptr; // nothing found
        return &*it;
    }

    void buildGeneratedSorted()
    {
        std::sort(m_data.entries().begin(), m_data.entries().end(), intern::compareGeneratedEntries<Entry>);
    }

    void buildOriginalNames() const
    {
        Q_ASSERT(m_originalNames.empty());
        // avoid using a std::set - the files list should be stable
        for (auto& entry : m_data.entries()) {
            if (entry.original.name.isEmpty()) continue;
            if (std::none_of(m_originalNames.begin(), m_originalNames.end(),
                             std::bind1st(std::equal_to<QString>(), entry.original.name))) {
                m_originalNames.push_back(entry.original.name);
            }
        }
    }

private:
    Data m_data;
    mutable QStringList m_originalNames;
};

template< typename... ExtensionTypes >
Mapping< ExtensionTypes... >::Mapping(Data data)
    : m_private(new Private(std::move(data)))
{}

template< typename... ExtensionTypes >
const typename Mapping< ExtensionTypes... >::Data&
Mapping< ExtensionTypes... >::data() const
{
    return m_private->data();
}

template< typename... ExtensionTypes >
const typename Mapping< ExtensionTypes... >::EntryList&
Mapping< ExtensionTypes... >::entriesSortedToGeneratedPosition() const
{
    return m_private->data().entries();
}

template< typename... ExtensionTypes >
const QStringList &
Mapping< ExtensionTypes... >::originalNames() const
{
    return m_private->originalNames();
}

template< typename... ExtensionTypes >
const typename Mapping< ExtensionTypes... >::Entry*
Mapping< ExtensionTypes... >::findEntryByGenerated(const Position &position) const
{
    if (!position.isValid()) return nullptr;
    return m_private->findEntryByGenerated(position);
}

} // namespace SourceMap

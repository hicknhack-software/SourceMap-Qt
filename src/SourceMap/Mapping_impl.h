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
#include "SourceMap/Mapping.h"

#include <functional>
#include <algorithm>

namespace SourceMap {

namespace intern {

template< typename Entry >
auto compareGeneratedRight(const Position &left, const Entry &right) -> int
{
    return !comparePositions(left, right.generated);
}

template< typename Entry >
auto compareGeneratedLeft(const Entry &left, const Position &right) -> int
{
    return !comparePositions(left.generated, right);
}

template< typename Entry >
auto compareGeneratedEntries(const Entry &left, const Entry &right) -> int
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
    using Extensions = SourceMap::Extensions< ExtensionTypes... >;

    Private(Data data)
        : m_data(std::move(data))
    {
        buildGeneratedSorted();
    }

    auto data() const -> const Data&
    {
        return m_data;
    }

    auto originalNames() const -> QStringList &
    {
        if (m_originalNames.empty()) buildOriginalNames();
        return m_originalNames;
    }

    auto findEntryByGenerated(const Position& position) -> const Entry*
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
        auto add = [&](const QString& name) {
            if (name.isEmpty()) return;
            if (std::none_of(m_originalNames.begin(), m_originalNames.end(), [&](const QString &v) {return v == name;})) {
                m_originalNames.push_back(name);
            }
        };
        for (auto &entry : m_data.entries()) add(entry.original.name);
        Extensions::collectFileNames(m_data, add);
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
auto Mapping< ExtensionTypes... >::data() const -> const typename Mapping< ExtensionTypes... >::Data&
{
    return m_private->data();
}

template< typename... ExtensionTypes >
auto Mapping< ExtensionTypes... >::entriesSortedToGeneratedPosition() const -> const typename Mapping< ExtensionTypes... >::EntryList&
{
    return m_private->data().entries();
}

template< typename... ExtensionTypes >
auto Mapping< ExtensionTypes... >::originalNames() const -> const QStringList &
{
    return m_private->originalNames();
}

template< typename... ExtensionTypes >
auto Mapping< ExtensionTypes... >::findEntryByGenerated(const Position &position) const -> const typename Mapping< ExtensionTypes... >::Entry*
{
    if (!position.isValid()) return nullptr;
    return m_private->findEntryByGenerated(position);
}

} // namespace SourceMap

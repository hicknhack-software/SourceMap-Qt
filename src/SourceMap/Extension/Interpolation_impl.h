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
#include "Interpolation.h"

#include "SourceMap/Entry.h"

#include <QJsonObject>
#include <vector>

namespace SourceMap {

template< typename... ExtensionTypes>
FilePosition getOriginalPositionFromGenerated(const Mapping< ExtensionTypes... >& mapping, const Position &position)
{
    using Entry = typename Mapping< ExtensionTypes... >::Entry;

    const Entry* entry = mapping.findEntryByGenerated(position);
    if (entry == nullptr)
        return {};

    if (get<Extension::Interpolation>(*entry) == Interpolation::None)
        return entry->original;

    int line = (position.line - entry->generated.line) + entry->original.line;
    int column = (position.line == entry->generated.line)
            ? (position.column - entry->generated.column) + entry->original.column
            : position.column;

    return {entry->original.name, {line, column}};
}

namespace Extension {
namespace intern {

/// Pair of Interpolation state and it's repetition count
using InterpolationList = std::vector< std::pair<SourceMap::Interpolation,int> >;

InterpolationList jsonDecodeInterpolationList(const RevisionThree &json);
void jsonStoreInterpolations(std::reference_wrapper<RevisionThree> json, const InterpolationList &interpolations);

template< typename Mapping >
InterpolationList extractInterpolationList(const Mapping& mapping)
{
    using Interpolation = SourceMap::Interpolation;
    InterpolationList result;
    auto interpolation = Interpolation::None;
    int repeated = 0;
    for (const auto& entry : mapping.data().entries) {
        const auto entryInterpolation = SourceMap::get<SourceMap::Extension::Interpolation>(entry);
        if (entryInterpolation != interpolation) {
            if (0 != repeated) result.emplace_back(interpolation, repeated);
            repeated = 0;
            interpolation = entryInterpolation;
        }
        repeated++;
    }
    if (0 != repeated && interpolation != Interpolation::None)
        result.emplace_back(interpolation, repeated);
    return result;
}

template< typename Data >
void injectInterpolationList(std::reference_wrapper<Data> data, intern::InterpolationList &&list)
{
    auto begin = list.begin();
    auto end = list.end();
    for (auto &entry : data.get().entries) {
        SourceMap::Interpolation entryInterpolation{};
        if (begin != end) {
            entryInterpolation = begin->first;
            if (--begin->second <= 0) begin++;
        }
        SourceMap::get<SourceMap::Extension::Interpolation>(entry) = entryInterpolation;
    }
}

} // namespace intern

template< typename Mapping >
void Interpolation::jsonEncode(const Mapping& mapping, std::reference_wrapper<RevisionThree> json)
{
    intern::jsonStoreInterpolations(json, intern::extractInterpolationList(mapping));
}

template< typename Data >
bool Interpolation::jsonDecode(std::reference_wrapper<Data> data, const RevisionThree &json)
{
    intern::injectInterpolationList(data, intern::jsonDecodeInterpolationList(json));
    return true;
}

} // namespace Extension
} // namespace SourceMap

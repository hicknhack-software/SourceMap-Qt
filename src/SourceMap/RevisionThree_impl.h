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
#include "RevisionThree.h"

namespace SourceMap {

namespace intern {

extern const QString MAPPINGS_KEY;

void jsonStoreMappings(std::reference_wrapper<RevisionThree> json, QString&& mappings);

void jsonDecodeMappings(const RevisionThree& json,
                        const std::function< void (Position &&generated, FilePosition &&original, QString&& name)> callback);

struct MappingsEncoder {
    MappingsEncoder(const QStringList &&sources, const QStringList &&nameList)
        : sources(std::move(sources)), nameList(std::move(nameList))
    {}

    void add(const Position& generated, const FilePosition& original, const QString& name);

    QString build();

    QStringList groups;
    QStringList sources;
    QStringList nameList;
    int generatedLine = 1;
    int generatedColumn = 1;
    int sourceIndex = 0;
    int sourceLine = 1;
    int sourceColumn = 1;
    int nameIndex = 0;
    QStringList segments;
};

template< typename Mapping >
const typename Mapping::EntryList& extractEntryList(const Mapping& mapping)
{
    return mapping.entriesSortedToGeneratedPosition();
}

template< typename Mapping >
QStringList extractSources(const Mapping& mapping)
{
    return mapping.originalNames();
}

template< typename Mapping >
QStringList extractNameList(const Mapping& mapping)
{
    QStringList names;
    for (auto& entry : extractEntryList(mapping)) {
        if (entry.name.isEmpty()) continue;
        if (std::none_of(names.begin(), names.end(),
                         std::bind1st(std::equal_to<QString>(), entry.name))) {
            names.push_back(entry.name);
        }
    }
    return names;
}

template< typename Mapping >
QString encodeMappingString(const QStringList&& sources, const QStringList&& names, const Mapping& mapping)
{
    MappingsEncoder encoder(std::move(sources), std::move(names));
    for(const auto& entry : extractEntryList(mapping))
        encoder.add(entry.generated, entry.original, entry.name);
    return encoder.build();
}

} // namespace

template< typename Data >
Data RevisionThree::decodedMappings() const
{
    typename Data::EntryList entries;
    intern::jsonDecodeMappings(*this, [&](Position &&generated, FilePosition &&original, QString&& name){
        entries.emplace_back(std::move(generated), std::move(original), std::move(name));
    });
    Data data{std::move(entries)};
    Data::Extensions::jsonDecode(std::ref(data), *this);
    return data;
}

template< typename Mapping >
void RevisionThree::encodeMappings(const Mapping &mappings)
{
    const auto sources = intern::extractSources(mappings);
    storeSources(sources);
    const auto names = intern::extractNameList(mappings);
    storeNames(names);
    intern::jsonStoreMappings(std::ref(*this), intern::encodeMappingString(std::move(sources), std::move(names), mappings));
    Mapping::Extensions::jsonEncode(mappings, std::ref(*this));
}

} // namespace SourceMap

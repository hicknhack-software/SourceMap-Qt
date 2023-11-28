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

#include "SourceMap/intern/Base64VLQ.h"

#include <QJsonArray>

using namespace Qt::StringLiterals;

namespace SourceMap {

namespace {

const QString VERSION_KEY = u"version"_s;
const QString FILE_KEY = u"file"_s;
const QString SOURCE_ROOT_KEY = u"sourceRoot"_s;
const QString SOURCES_KEY = u"sources"_s;
const QString SOURCES_CONTENT_KEY = u"sourcesContent"_s;
const QString NAMES_KEY = u"names"_s;

const auto GROUP_DELIMITER = QChar{';'};
const auto SECTION_DELIMIER = QChar{','};

const QByteArray PROTECTION_SEQUENCE = ")]}"_ba;

} // namespace

namespace intern {

const QString MAPPINGS_KEY = u"mappings"_s;

void jsonStoreMappings(std::reference_wrapper<RevisionThree> json, QString&& mappings)
{
    json.get().insert(MAPPINGS_KEY, mappings);
}

void jsonDecodeMappings(const RevisionThree& json,
                        const std::function< void (Position &&generated, FilePosition &&original, QString&& name)> callback)
{
    const auto encodedMappings = json.value(MAPPINGS_KEY).toString();
    const auto sources = json.sources();
    const auto nameList = json.names();

    auto begin = encodedMappings.begin();
    auto end = encodedMappings.end();

    int generatedLine = 1;
    int generatedColumn = 1;
    int sourceIndex = 0;
    int sourceLine = 1;
    int sourceColumn = 1;
    int nameIndex = 0;
    const auto load = [&begin, end](int &reference, int value) {
        auto success = Base64VLQ::decode(begin, end, std::ref(value));
        if (success) reference = (value += reference);
        return value;
    };
    while (begin != end) {
        auto ch = *begin;
        if (GROUP_DELIMITER == ch) {
            generatedLine++;
            generatedColumn = 1;
            begin++;
            continue; // line break
        }
        if (SECTION_DELIMIER == ch) {
            begin++;
            continue; // section
        }
        const auto entryGeneratedColumn = load(std::ref(generatedColumn), 0);
        const auto entrySourceIndex = load(sourceIndex, -1);
        const auto entrySourceLine = load(sourceLine, 0);
        const auto entrySourceColumn = load(sourceColumn, 0);
        const auto entryNameIndex = load(nameIndex, -1);
        auto entryGenerated = Position{generatedLine, entryGeneratedColumn};
        auto entrySource = sources.value(entrySourceIndex);
        auto entryOriginal = FilePosition{std::move(entrySource), {entrySourceLine, entrySourceColumn}};
        auto entryName = nameList.value(entryNameIndex);
        callback(std::move(entryGenerated), std::move(entryOriginal), std::move(entryName));
    }
}


void MappingsEncoder::add(const Position &generated, const FilePosition &original, const QString &name)
{
    while(generatedLine != generated.line) {
        groups.push_back(segments.join(SECTION_DELIMIER));
        segments.clear();
        generatedLine++;
        generatedColumn = 1;
    }
    // TODO: avoid duplicate entries
    QString fields;
    auto store = [&](int current, int& previous){
        Base64VLQ::encode(std::ref(fields), current - previous);
        previous = current;
    };

    store(generated.column, generatedColumn);
    if (!original.name.isEmpty()) {
        int entrySourceIndex = sources.indexOf(original.name);
        store(entrySourceIndex, sourceIndex);
        store(original.line, sourceLine);
        store(original.column, sourceColumn);

        int entryNameIndex = name.isEmpty() ? -1 : nameList.indexOf(name);
        if (-1 != entryNameIndex) {
            store(entryNameIndex, nameIndex);
        }
    }
    segments.push_back(fields);
}

auto MappingsEncoder::build() -> QString
{
    if (!segments.isEmpty())
        groups.push_back(segments.join(SECTION_DELIMIER));
    return groups.join(GROUP_DELIMITER);
}

} // namespace intern

RevisionThree::RevisionThree()
{
    insert(VERSION_KEY, 3);
}

RevisionThree::RevisionThree(QJsonObject &&source)
    : QJsonObject(std::move(source))
{}

auto RevisionThree::version() const -> int
{
    return value(VERSION_KEY).toInt(0);
}

auto RevisionThree::file() const -> QString
{
    return value(FILE_KEY).toString();
}

void RevisionThree::storeFile(const QString &file)
{
    insert(FILE_KEY, file);
}

auto RevisionThree::sourceRoot() const -> QString
{
    return value(SOURCE_ROOT_KEY).toString();
}

void RevisionThree::storeSourceRoot(const QString &sourceRoot)
{
    insert(SOURCE_ROOT_KEY, sourceRoot);
}

auto RevisionThree::sources() const -> QStringList
{
    auto jsonArray = value(SOURCES_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeSources(const QStringList &sources)
{
    insert(SOURCES_KEY, QJsonArray::fromStringList(sources));
}

auto RevisionThree::sourcesContent() const -> QStringList
{
    auto jsonArray = value(SOURCES_CONTENT_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeSourcesContent(const QStringList &sourcesContent)
{
    insert(SOURCES_CONTENT_KEY, QJsonArray::fromStringList(sourcesContent));
}

auto RevisionThree::names() const -> QStringList
{
    auto jsonArray = value(NAMES_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeNames(const QStringList &names)
{
    insert(NAMES_KEY, QJsonArray::fromStringList(names));
}

auto RevisionThree::fromJson(const QByteArray &json, QJsonParseError *error) -> RevisionThree
{
    auto jsonCopy = json;
    if (jsonCopy.startsWith(PROTECTION_SEQUENCE))
        jsonCopy.remove(0, PROTECTION_SEQUENCE.size()); // remove any protection
    auto document = QJsonDocument::fromJson(jsonCopy, error);
    return RevisionThree{ document.object() };
}

auto RevisionThree::toJson(QJsonDocument::JsonFormat format) const -> QByteArray
{
    return QJsonDocument(*this).toJson(format);
}

} // namespace SourceMap

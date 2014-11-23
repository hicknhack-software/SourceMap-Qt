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

namespace SourceMap {

namespace {

const QString VERSION_KEY = "version";
const QString FILE_KEY = "file";
const QString SOURCE_ROOT_KEY = "sourceRoot";
const QString SOURCES_KEY = "sources";
const QString SOURCES_CONTENT_KEY = "sourcesContent";
const QString NAMES_KEY = "names";

const QChar GROUP_DELIMITER = ';';
const QChar SECTION_DELIMIER = ',';

const char* PROTECTION_SEQUENCE = ")]}";

} // namespace

namespace intern {

const QString MAPPINGS_KEY = "mappings";

void jsonStoreMappings(std::reference_wrapper<RevisionThree> json, QString&& mappings)
{
    json.get().insert(MAPPINGS_KEY, mappings);
}

void jsonDecodeMappings(const RevisionThree& json,
                        const std::function< void (Position &&generated, FilePosition &&original, QString&& name)> callback)
{
    const QString encodedMappings = json.value(MAPPINGS_KEY).toString();
    const QStringList sources = json.sources();
    const QStringList nameList = json.names();

    auto begin = encodedMappings.begin();
    auto end = encodedMappings.end();

    int generatedLine = 1;
    int generatedColumn = 1;
    int sourceIndex = 0;
    int sourceLine = 1;
    int sourceColumn = 1;
    int nameIndex = 0;
    while (begin != end) {
        QChar ch = *begin;
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
        auto load = [&begin, end](int &reference, int value) {
            bool success = Base64VLQ::decode(begin, end, std::ref(value));
            if (success) reference = (value += reference);
            return value;
        };
        int entryGeneratedColumn = load(std::ref(generatedColumn), 0);
        int entrySourceIndex = load(sourceIndex, -1);
        int entrySourceLine = load(sourceLine, 0);
        int entrySourceColumn = load(sourceColumn, 0);
        int entryNameIndex = load(nameIndex, -1);
        auto entryGenerated = Position{generatedLine, entryGeneratedColumn};
        auto entrySource = sources.value(entrySourceIndex);
        auto entryOriginal = FilePosition{entrySource, {entrySourceLine, entrySourceColumn}};
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
    auto store = [&fields](int current, int& previous){
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

QString MappingsEncoder::build()
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

int RevisionThree::version() const
{
    return value(VERSION_KEY).toInt(0);
}

QString RevisionThree::file() const
{
    return value(FILE_KEY).toString();
}

void RevisionThree::storeFile(const QString &file)
{
    insert(FILE_KEY, file);
}

QString RevisionThree::sourceRoot() const
{
    return value(SOURCE_ROOT_KEY).toString();
}

void RevisionThree::storeSourceRoot(const QString &sourceRoot)
{
    insert(SOURCE_ROOT_KEY, sourceRoot);
}

QStringList RevisionThree::sources() const
{
    QJsonArray jsonArray = value(SOURCES_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeSources(const QStringList &sources)
{
    insert(SOURCES_KEY, QJsonArray::fromStringList(sources));
}

QStringList RevisionThree::sourcesContent() const
{
    QJsonArray jsonArray = value(SOURCES_CONTENT_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeSourcesContent(const QStringList &sourcesContent)
{
    insert(SOURCES_CONTENT_KEY, QJsonArray::fromStringList(sourcesContent));
}

QStringList RevisionThree::names() const
{
    QJsonArray jsonArray = value(NAMES_KEY).toArray();
    QStringList result;
    result.reserve(jsonArray.size());
    for (const auto &value : jsonArray) result.push_back(value.toString());
    return result;
}

void RevisionThree::storeNames(const QStringList &names)
{
    insert(NAMES_KEY, QJsonArray::fromStringList(names));
}

RevisionThree RevisionThree::fromJson(const QByteArray &json, QJsonParseError *error)
{
    QByteArray jsonCopy = json;
    if (jsonCopy.startsWith(PROTECTION_SEQUENCE))
        jsonCopy.remove(0, strlen(PROTECTION_SEQUENCE)); // remove any protection
    auto document = QJsonDocument::fromJson(jsonCopy, error);
    return RevisionThree{ document.object() };
}

QByteArray RevisionThree::toJson(QJsonDocument::JsonFormat format) const
{
    return QJsonDocument(*this).toJson(format);
}

} // namespace SourceMap

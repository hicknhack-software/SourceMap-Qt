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

#include "SourceMap/Mapping.h"

#include <QJsonObject>
#include <QJsonDocument>

#include <vector>

namespace SourceMap {

/**
 * @brief Qt implementation of the "Source Map Revision 3 Proposal" from Mozilla & Google
 *
 * https://docs.google.com/document/d/1U1RGAehQwRypUTovF1KRlpiOFze0b-_2gc6fAH0KY0k/edit?pli=1#heading=h.1ce2c87bpj24
 *
 * You should only add additional values according to the standard!
 */
class RevisionThree : public QJsonObject
{
public:
    /// Creates the construct with version 3
    RevisionThree();

    explicit RevisionThree(QJsonObject&& source);

    /// @returns stored version value
    int version() const;

    /// @returns stored generated file name (might be empty)
    QString file() const;
    void storeFile(const QString& file); ///< stores the name of the generated file

    /// @returns stored base url/path for all sources
    QString sourceRoot() const;
    void storeSourceRoot(const QString& sourceRoot); ///< stores base url/path for all sources

    /// @returns stored names for all sources relative to sourceRoot (indexed from mappings)
    QStringList sources() const;
    void storeSources(const QStringList& sources); ///< stores names for all sources relative to sourceRoot (indexed from mappings, called by setMapping)

    /// @returns stored content for all sources (has to correlate stored names for the sources or stay empty)
    QStringList sourcesContent() const;
    void storeSourcesContent(const QStringList& sourcesContent); ///< stores content for all sources (has to correlate stored names for the sources or stay empty)

    /// @returns stored (symbol) names (indexed from the mappings)
    QStringList names() const;
    void storeNames(const QStringList& names); ///< stores the (symbol) names (indexed from the mappings, called by setMapping)

    /// @returns decoded version of mappings (includes sources, names and extensions)
    template< typename Data >
    Data decodedMappings() const;

    /// stores the encoded mappings together with sources, names and interpolations
    template< typename Mapping >
    void encodeMappings(const Mapping& mapping);

    /// creates the store from a JSON representations (also removes protections)
    static RevisionThree fromJson(const QByteArray &json, QJsonParseError* error = 0);

    /// creates a JSON representation of the stored data
    QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;
};

} // namespace SourceMap

#include "RevisionThree_impl.h"


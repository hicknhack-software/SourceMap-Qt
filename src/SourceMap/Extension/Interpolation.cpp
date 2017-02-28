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

#include "SourceMap/intern/Base64VLQ.h"

#include "SourceMap/RevisionThree.h"

namespace SourceMap {
namespace Extension {
namespace intern {

namespace {

const auto SEGMENT_DELIMITER = QChar{','};
const auto INTERPOLATION_DELIMITER = QChar{';'};
const auto INTERPOLATIONS_KEY = QString{"x_hicknhack_interpolations"};
const auto COLUMN_INTERPOLATIONS_KEY = QString{"x_de_hicknhack_software_column_interpolation"};

QString encodeInterpolations(const CompressedInterpolationList &interpolations)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    QString encoded;
    for(const auto& p : interpolations) {
        Base64VLQ::encode(encoded, static_cast<int>(p.first));
        Base64VLQ::encode(encoded, p.second);
    }
    return encoded;
}

QString encodeGeneratedLineInterpolations(const GeneratedLineInterpolationList &interpolations)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    QString encoded;
    auto lastLine = 1;
    auto newLine = true;
    for (const auto &p : interpolations) {
        const auto line = std::get<0>(p);
        const auto interpolation = std::get<1>(p);

        const auto lineDiff = (line - lastLine);
        for (auto i = 0; i < lineDiff; ++i) {
            encoded.append(INTERPOLATION_DELIMITER);
        }

        if (lineDiff > 0) {
            lastLine = line;
            newLine = true;
        }

        if (lineDiff == 0 && newLine == false) {
            encoded.append(SEGMENT_DELIMITER);
        }

        Base64VLQ::encode(encoded, static_cast<int>(interpolation));
        newLine = false;
    }
    return encoded;
}

} // namespace

CompressedInterpolationList jsonDecodeCompressedInterpolationList(const RevisionThree& jsonObject)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    CompressedInterpolationList result;
    const auto encoded = jsonObject.value(INTERPOLATIONS_KEY).toString();
    auto begin = encoded.begin();
    const auto end = encoded.end();
    while (begin != end) {
        auto interpolation = static_cast<SourceMap::Interpolation>(Base64VLQ::decode(std::ref(begin), end, 0));
        auto count = Base64VLQ::decode(std::ref(begin), end, 0);
        result.emplace_back(interpolation, count);
    }
    return result;
}

void jsonStoreInterpolations(std::reference_wrapper<RevisionThree> json, const CompressedInterpolationList &interpolations)
{
    if (interpolations.empty()) return; // nothing to store

    const auto encodedInterpolations = encodeInterpolations(interpolations);
    json.get().insert(INTERPOLATIONS_KEY, encodedInterpolations);
}

InterpolationList jsonDecodeGeneratedLineInterpolationList(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    InterpolationList result;
    const auto encoded = json.value(COLUMN_INTERPOLATIONS_KEY).toString();
    auto begin = encoded.begin();
    const auto end = encoded.end();
    while (begin != end) {
        if (*begin == INTERPOLATION_DELIMITER || *begin == SEGMENT_DELIMITER) {
            ++begin;
            continue;
        }
        auto interpolation = static_cast<SourceMap::Interpolation>(Base64VLQ::decode(std::ref(begin), end, 0));
        result.emplace_back(interpolation);
    }
    return result;
}

void jsonStoreColumnFormatInterpolations(std::reference_wrapper<RevisionThree> json, const GeneratedLineInterpolationList &interpolations)
{
    const auto encodedInterpolations = encodeGeneratedLineInterpolations(interpolations);
    json.get().insert(COLUMN_INTERPOLATIONS_KEY, encodedInterpolations);
}

} // namespace intern
} // namespace Extension
} // namespace SourceMap

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

const QString INTERPOLATIONS_KEY = "x_hicknhack_interpolations";

} // namespace

InterpolationList jsonDecodeInterpolationList(const RevisionThree& jsonObject)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    InterpolationList result;

    const QString encoded = jsonObject.value(INTERPOLATIONS_KEY).toString();
    auto begin = encoded.begin();
    auto end = encoded.end();
    while (begin != end) {
        auto interpolation = static_cast<SourceMap::Interpolation>(Base64VLQ::decode(std::ref(begin), end, 0));
        int count = Base64VLQ::decode(std::ref(begin), end, 0);
        result.emplace_back(interpolation, count);
    }
    return result;
}

void jsonStoreInterpolations(std::reference_wrapper<RevisionThree> json, const InterpolationList &interpolations)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    if (interpolations.empty()) return; // nothing to store

    QString encoded;
    for(const auto& p : interpolations) {
        Base64VLQ::encode(encoded, static_cast<int>(p.first));
        Base64VLQ::encode(encoded, p.second);
    }
    json.get().insert(INTERPOLATIONS_KEY, encoded);
}

} // namespace intern
} // namespace Extension
} // namespace SourceMap

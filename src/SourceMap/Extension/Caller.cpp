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
#include "Caller.h"

#include "SourceMap/intern/Base64VLQ.h"

#include "SourceMap/RevisionThree.h"

namespace SourceMap {
namespace Extension {
namespace intern {

namespace {

const QChar CALLER_DELIMITER = ';';

const QString CALLER_INDICES_KEY = "x_hicknhack_caller_indices";
const QString CALLERS_KEY = "x_hicknhack_callers";

} // namespace

CallerStack buildCallerStack(const CallerList& callers, CallerIndex index)
{
    CallerStack stack;
    while (InvalidCallerIndex != index.value) {
        stack.push_back(callers[index.value].original);
        index = callers[index.value].parentIndex;
    }
    return stack;
}

CallerIndexList jsonDecodeCallerIndices(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    CallerIndexList result;

    const QString encoded = json.value(CALLER_INDICES_KEY).toString();
    auto begin = encoded.begin();
    auto end = encoded.end();
    while (begin != end) {
        int caller_index = Base64VLQ::decode(std::ref(begin), end, 0);
        result.push_back(CallerIndex{caller_index});
    }
    return result;
}

void jsonStoreCallerIndices(std::reference_wrapper<RevisionThree> json,
                            const CallerIndexList& callerIndices)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    if (callerIndices.empty()) return; // nothing to store

    QString encoded;
    for(auto p : callerIndices) {
        Base64VLQ::encode(encoded, p.value);
    }
    json.get().insert(CALLER_INDICES_KEY, encoded);
}

CallerList jsonDecodeCallerList(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    CallerList result;
    const QStringList sources = json.sources();
    const QString encoded = json.value(CALLERS_KEY).toString();

    auto begin = encoded.begin();
    const auto end = encoded.end();
    while (begin != end) {
        if (*begin == CALLER_DELIMITER) {
            begin++;
            continue;
        }
        SourceMap::Caller caller;
        int sourceIndex = Base64VLQ::decode(std::ref(begin), end, -1);
        if (sourceIndex != -1) {
            caller.original.name = sources.value(sourceIndex);
            caller.original.line = Base64VLQ::decode(std::ref(begin), end, 0);
            caller.original.column = Base64VLQ::decode(std::ref(begin), end, 0);
        }
        caller.parentIndex = CallerIndex { Base64VLQ::decode(std::ref(begin), end, -1) };
        result.push_back(std::move(caller));
    }
    return result;
}

void jsonStoreCallerList(std::reference_wrapper<RevisionThree> json, const CallerList& callers)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    if (callers.empty()) return; // nothing to store

    const QStringList sources = json.get().sources();
    QString encoded;
    for (const SourceMap::Caller& caller : callers) {
        int sourceIndex = sources.indexOf(caller.original.name);
        Base64VLQ::encode(encoded, sourceIndex);
        if (-1 != sourceIndex) {
            Base64VLQ::encode(encoded, caller.original.line);
            Base64VLQ::encode(encoded, caller.original.column);
        }
        if (InvalidCallerIndex != caller.parentIndex.value)
            Base64VLQ::encode(encoded, caller.parentIndex.value);
        encoded.append(CALLER_DELIMITER);
    }
    json.get().insert(CALLERS_KEY, encoded);
}

} // namespace intern
} // namespace Extension
} // namespace SourceMap

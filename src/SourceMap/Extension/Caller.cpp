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

const auto CALLER_DELIMITER = QChar{';'};
const auto CALLER_INDICES_KEY = QString{"x_hicknhack_caller_indices"};
const auto CALLERS_KEY = QString{"x_hicknhack_callers"};

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
    const auto encoded = json.value(CALLER_INDICES_KEY).toString();
    auto begin = encoded.begin();
    const auto end = encoded.end();
    auto lastIndex = 0;
    while (begin != end) {
        auto index = InvalidCallerIndex;
        auto success = Base64VLQ::decode(begin, end, std::ref(index));
        if (success) lastIndex = (index += lastIndex);
        result.push_back(CallerIndex{index});
    }
    return result;
}

void jsonStoreCallerIndices(std::reference_wrapper<RevisionThree> json,
                            const CallerIndexList& callerIndices)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    if (callerIndices.empty()) return; // nothing to store

    QString encoded;
    auto lastIndex = 0;
    for(auto p : callerIndices) {
        Base64VLQ::encode(encoded, p.value - lastIndex);
        lastIndex = p.value;
    }
    json.get().insert(CALLER_INDICES_KEY, encoded);
}

CallerList jsonDecodeCallerList(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    CallerList result;
    const auto sources = json.sources();
    const auto encoded = json.value(CALLERS_KEY).toString();

    auto begin = encoded.begin();
    const auto end = encoded.end();
    auto sourceIndex = 0;
    auto sourceLine = 1;
    auto sourceColumn = 1;
    auto parentIndex = 0;
    const auto load = [&begin, end](std::reference_wrapper<int> reference, int value) {
        bool success = Base64VLQ::decode(begin, end, std::ref(value));
        if (success) reference.get() = (value += reference.get());
        return value;
    };
    while (begin != end) {
        if (*begin == CALLER_DELIMITER) {
            begin++;
            continue;
        }
        auto caller = SourceMap::Caller{};
        const auto callerSourceIndex = load(std::ref(sourceIndex), -1);
        if (callerSourceIndex != -1) {
            caller.original.name = sources.value(callerSourceIndex);
            caller.original.line = load(std::ref(sourceLine), 0);
            caller.original.column = load(std::ref(sourceColumn), 0);
        }
        caller.parentIndex = CallerIndex{ load(std::ref(parentIndex), InvalidCallerIndex) };
        result.push_back(std::move(caller));
    }
    return result;
}

void jsonStoreCallerList(std::reference_wrapper<RevisionThree> json, const CallerList& callers)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;
    if (callers.empty()) return; // nothing to store

    const auto sources = json.get().sources();
    QString encoded;
    auto sourceIndex = 0;
    auto sourceLine = 1;
    auto sourceColumn = 1;
    auto parentIndex = 0;
    auto store = [&](int current, int& previous){
        Base64VLQ::encode(std::ref(encoded), current - previous);
        previous = current;
    };
    for (const SourceMap::Caller& caller : callers) {
        auto callerSourceIndex = sources.indexOf(caller.original.name);
        store(callerSourceIndex, sourceIndex);
        if (-1 != callerSourceIndex) {
            store(caller.original.line, sourceLine);
            store(caller.original.column, sourceColumn);
        }
        if (InvalidCallerIndex != caller.parentIndex.value)
            store(caller.parentIndex.value, parentIndex);
        encoded.append(CALLER_DELIMITER);
    }
    json.get().insert(CALLERS_KEY, encoded);
}

} // namespace intern
} // namespace Extension
} // namespace SourceMap

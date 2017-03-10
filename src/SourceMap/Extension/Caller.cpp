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

const auto SEGMENT_DELIMITER = QChar{','};
const auto CALLER_DELIMITER = QChar{';'};
const auto CALLSTACK_FORMAT_KEY = QString{"x_de_hicknhack_software_callstack"};
const auto CALLERS_KEY = QString{"callers"};
const auto CALLER_INDICES_KEY = QString{"indices"};

QString encodeCallerIndices(const GeneratedLineCallerIndexList& callerIndices)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    QString encoded;
    auto lastIndex = 0;
    auto lastLine = 1;
    auto newLine = true;
    for(const auto &p : callerIndices) {
        const auto line = std::get<0>(p);
        const auto index = std::get<1>(p);

        const auto lineDiff = (line - lastLine);
        for (auto i = 0; i < lineDiff; ++i) {
            encoded.append(CALLER_DELIMITER);
        }

        if (lineDiff > 0) {
            lastLine = line;
            newLine = true;
        }

        if (lineDiff == 0 && newLine == false) {
            encoded.append(SEGMENT_DELIMITER);
        }

        if (index.value != -1) {
            Base64VLQ::encode(encoded, index.value - lastIndex);
            lastIndex = index.value;
        }

        newLine = false;
    }
    return encoded;
}

QString encodeCallerList(const CallerList &callers, const QStringList &sources)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

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
        if (encoded.length() > 0) {
            encoded.append(CALLER_DELIMITER);
        }
        auto callerSourceIndex = sources.indexOf(caller.original.name);
        store(callerSourceIndex, sourceIndex);
        store(caller.original.line, sourceLine);
        store(caller.original.column, sourceColumn);
        if (InvalidCallerIndex != caller.parentIndex.value) {
            store(caller.parentIndex.value, parentIndex);
        }
    }
    return encoded;
}

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

void jsonStoreCallers(std::reference_wrapper<RevisionThree> json, const CallerList &callers, const GeneratedLineCallerIndexList &callerIndices)
{
    QJsonObject callerObject;
    const auto sources = json.get().sources();
    const auto encodedCallers = encodeCallerList(callers, sources);
    callerObject.insert(CALLERS_KEY, encodedCallers);

    const auto encodedIndices = encodeCallerIndices(callerIndices);
    callerObject.insert(CALLER_INDICES_KEY, encodedIndices);

    json.get().insert(CALLSTACK_FORMAT_KEY, callerObject);
}

CallerIndexList jsonDecodeCallerIndices(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto callerObject = json.value(CALLSTACK_FORMAT_KEY).toObject();
    CallerIndexList result;
    const auto encoded = callerObject.value(CALLER_INDICES_KEY).toString();
    auto begin = encoded.begin();
    const auto end = encoded.end();
    auto lastIndex = 0;
    while (begin != end) {
        if (*begin == CALLER_DELIMITER || *begin == SEGMENT_DELIMITER) {
            ++begin;
            continue;
        }
        auto index = InvalidCallerIndex;
        auto success = Base64VLQ::decode(begin, end, std::ref(index));
        if (success) lastIndex = (index += lastIndex);
        result.push_back(CallerIndex{index});
    }
    return result;
}

CallerList jsonDecodeCallerList(const RevisionThree &json)
{
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    CallerList result;
    const auto sources = json.sources();
    const auto callerObject = json.value(CALLSTACK_FORMAT_KEY).toObject();
    const auto encoded= callerObject.value(CALLERS_KEY).toString();

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
        caller.original.name = sources.value(callerSourceIndex);
        caller.original.line = load(std::ref(sourceLine), 0);
        caller.original.column = load(std::ref(sourceColumn), 0);
        caller.parentIndex = CallerIndex{ load(std::ref(parentIndex), InvalidCallerIndex) };
        result.push_back(std::move(caller));
    }
    return result;
}

} // namespace intern
} // namespace Extension
} // namespace SourceMap

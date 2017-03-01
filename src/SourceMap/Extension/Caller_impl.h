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

#include "SourceMap/Entry.h"
#include "SourceMap/Data.h"

namespace SourceMap {
namespace Extension {
namespace intern {

CallerStack buildCallerStack(const CallerList &callers, CallerIndex index);

using CallerIndexList = std::vector< CallerIndex >;
using GeneratedLineCallerIndexList = std::vector< std::tuple<int, CallerIndex> >;

CallerIndexList jsonDecodeCallerIndices(const RevisionThree &json);
void jsonStoreCallerIndices(std::reference_wrapper<RevisionThree> json, const CallerIndexList& callerIndices);

CallerList jsonDecodeCallerList(const RevisionThree &json);
void jsonStoreCallerList(std::reference_wrapper<RevisionThree> json, const CallerList& callers);

CallerIndexList jsonDecodeCallstackCallerIndices(const RevisionThree &json);
CallerList jsonDecodeCallstackCallerList(const RevisionThree &json);
void jsonStoreCallstackFormatCaller(std::reference_wrapper<RevisionThree> json, const CallerList &callers, const GeneratedLineCallerIndexList& callerIndices);

template< typename Mapping >
CallerIndexList extractCallerIndices(const Mapping& mapping)
{
    CallerIndexList result;
    const auto &entries = mapping.data().entries();
    result.reserve(entries.size());
    for (const auto& entry : entries) {
        const auto callerIndex = SourceMap::get<SourceMap::Extension::Caller>(entry);
        result.push_back(callerIndex);
    }
    return result;
}

template< typename Mapping >
GeneratedLineCallerIndexList extractGeneratedLineCallerIndices(const Mapping& mapping)
{
    GeneratedLineCallerIndexList result;
    const auto &entries = mapping.data().entries();
    result.reserve(entries.size());
    for (const auto& entry : entries) {
        const auto callerIndex = SourceMap::get<SourceMap::Extension::Caller>(entry);
        result.push_back(std::make_tuple(entry.generated.line, callerIndex));
    }
    return result;
}

template< typename Mapping >
SourceMap::CallerList extractCallerList(const Mapping& mapping)
{
    return SourceMap::get<SourceMap::Extension::Caller>(mapping.data());
}

template< typename Data >
void injectCallerIndices(std::reference_wrapper<Data> data, intern::CallerIndexList &&list)
{
    auto begin = list.begin();
    auto end = list.end();
    for (auto &entry : data.get().entries()) {
        SourceMap::get<SourceMap::Extension::Caller>(entry) = *begin++;
        if (begin == end) break;
    }
}

template< typename Data >
void injectCallerList(std::reference_wrapper<Data> data, SourceMap::CallerList &&list)
{
    SourceMap::get<SourceMap::Extension::Caller>(data.get()) = std::move(list);
}

} // namespace intern

template< typename Mapping >
void Caller::jsonEncode(const Mapping& mapping, std::reference_wrapper<RevisionThree> json)
{
    intern::jsonStoreCallerIndices(json, intern::extractCallerIndices(mapping));
    intern::jsonStoreCallerList(json, intern::extractCallerList(mapping));
}

template< typename Data >
bool Caller::jsonDecode(std::reference_wrapper<Data> data, const RevisionThree &json)
{
    intern::injectCallerIndices(data, intern::jsonDecodeCallerIndices(json));
    intern::injectCallerList(data, intern::jsonDecodeCallerList(json));
    return true;
}

template< typename Mapping >
void CallstackFormatCaller::jsonEncode(const Mapping& mapping, std::reference_wrapper<RevisionThree> json)
{
    intern::jsonStoreCallstackFormatCaller(json, intern::extractCallerList(mapping), intern::extractGeneratedLineCallerIndices(mapping));
}

template< typename Data >
bool CallstackFormatCaller::jsonDecode(std::reference_wrapper<Data> data, const RevisionThree &json)
{
    intern::injectCallerIndices(data, intern::jsonDecodeCallstackCallerIndices(json));
    intern::injectCallerList(data, intern::jsonDecodeCallstackCallerList(json));
    return true;
}

} // namespace Extension

template< typename... ExtensionTypes >
CallerStack buildCallerStack(const Data< ExtensionTypes... > &data,
                             const Entry< ExtensionTypes... > *entry)
{
    if (nullptr == entry) return CallerStack{};
    return Extension::intern::buildCallerStack(get<Extension::Caller>(data), get<Extension::Caller>(*entry));
}

template< typename... ExtensionTypes >
CallerStack buildCallerStack(const Mapping< ExtensionTypes... > &mapping,
                             const Entry< ExtensionTypes... > *entry)
{
    return buildCallerStack(mapping.data(), entry);
}

} // namespace SourceMap

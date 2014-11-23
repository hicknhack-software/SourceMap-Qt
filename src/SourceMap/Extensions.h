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
#ifndef SOURCEMAP_EXTENSIONS_H
#define SOURCEMAP_EXTENSIONS_H

#include "SourceMap/meta/tuple_without_void.h"
#include "SourceMap/meta/concat_tuples.h"

#include "SourceMap/Extension/Base.h"

#include <tuple>

namespace SourceMap {

template< typename... ExtensionTypes >
struct Entry;

template< typename... ExtensionTypes >
struct Data;

template< typename... ExtensionTypes >
class Mapping;

class RevisionThree;

/// This structure helps to manage an extension pack
template< typename... ExtensionTypes >
struct Extensions
{
    using EntryData = meta::tuple_without_void_t< typename ExtensionTypes::EntryData ... >;
    using MapData = meta::tuple_without_void_t< typename ExtensionTypes::MapData ... >;

    using Entry = SourceMap::Entry< ExtensionTypes... >;
    using Data = SourceMap::Data< ExtensionTypes... >;
    using Mapping = SourceMap::Mapping< ExtensionTypes... >;

    /// calls jsonEncode for every extension
    static void jsonEncode(const Mapping& mapping, std::reference_wrapper<RevisionThree> jsonObject)
    {
        using JsonEncode = void (*)(const Mapping& mapping, std::reference_wrapper<RevisionThree> jsonObject);
        const JsonEncode jsonEncoders[] = {
            nullptr, &ExtensionTypes::template jsonEncode<Mapping> ...
        };
        for(auto jsonEncoder : jsonEncoders) if (jsonEncoder)
            jsonEncoder(mapping, jsonObject);
    }

    /// calls jsonDecode for every extension
    static bool jsonDecode(std::reference_wrapper<Data> data, const RevisionThree &jsonObject)
    {
        using JsonDecode = bool (*)(std::reference_wrapper<Data> data, const RevisionThree &jsonObject);
        const JsonDecode jsonDecoders[] = {
            nullptr, &ExtensionTypes::template jsonDecode<Data> ...
        };
        for(auto jsonDecoder : jsonDecoders) if (jsonDecoder) {
            const bool success = jsonDecoder(data, jsonObject);
            if (!success) return false; // decoding failed
        }
        return true;
    }
};

} // namespace SourceMap

#endif // SOURCEMAP_EXTENSIONS_H

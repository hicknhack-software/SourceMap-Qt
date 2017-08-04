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

#include <tuple>
#include <functional>

namespace SourceMap {

class RevisionThree;

namespace Extension {

/// Base Schema of a valid SourceMap extension
struct Base
{
    using EntryData = void; ///< additional Entry data members
    using MapData = void; ///< additional Mapping data members

    /// Mapping encode function
    template< typename Data >
    static void jsonEncode(const Data&, std::reference_wrapper<RevisionThree>);

    /// @returns false if decoding failed
    template< typename Mapping >
    static bool jsonDecode(std::reference_wrapper<Mapping>, const RevisionThree&);

    /// Invoke Callback with additional FileNames
    ///
    /// * optional (default will not report any file names
    template< typename Data, typename Callback >
    static void collectFileNames(const Data&, Callback&) {}
};

} // namespace Extension
} // namespace SourceMap

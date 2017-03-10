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

#include "SourceMap/Extension/Base.h"

#include "SourceMap/FilePosition.h"

namespace SourceMap {

template< typename... ExtensionTypes>
class Mapping;

/// Flags wheter source interpolation is possible
enum class Interpolation {
    None, ///< no interpolation possible
    OneToOne ///< 1:1 from source - interpolate the columns
};

/// @returns invalid Position for invalid position
/// interpolated position if interpolation is allowed
/// start of the entry before if no interpolation is allowed
template< typename... ExtensionTypes>
FilePosition getOriginalPositionFromGenerated(const Mapping< ExtensionTypes... >& mapping, const Position &position);

namespace Extension {

/// This implements the Extension interface for the Interpolation
struct Interpolation : Base
{
    using EntryData = SourceMap::Interpolation;

    /**
     * @brief Adds x_hicknhack_interpolations json entry
     *
     * It encodes the value of the Interpolation Flag for each mapping.
     * * The String is base 64 VLQ encoded pairs.
     * * First the Value of the Flag.
     * * Second the ammount of repetitions for this value.
     */
    template< typename Mapping >
    static void jsonEncode(const Mapping&, std::reference_wrapper<RevisionThree>);

    /**
     * @brief Decodes x_hicknhack_interpolations json entry
     *
     * If it's missing no interpolations are done (no error is raised)
     */
    template< typename Data >
    static bool jsonDecode(std::reference_wrapper<Data>, const RevisionThree&);
};

} // namespace Extension
} // namespace SourceMap

#include "Interpolation_impl.h"


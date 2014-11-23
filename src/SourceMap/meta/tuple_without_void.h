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
#ifndef SOURCEMAP_META_TUPLE_WITHOUT_VOID_H
#define SOURCEMAP_META_TUPLE_WITHOUT_VOID_H

#include <tuple>

namespace SourceMap {
namespace meta {

template< typename tuple, typename... Types >
struct tuple_without_void;

template< typename... Types >
struct tuple_without_void< std::tuple<Types...> >
{
    using type = std::tuple<Types...>;
};

template< typename... Types, typename... Tail >
struct tuple_without_void< std::tuple<Types...>, void, Tail... >
{
    using type = typename tuple_without_void< std::tuple<Types...>, Tail... >::type;
};

template< typename... Types, typename T, typename... Tail >
struct tuple_without_void< std::tuple<Types...>, T, Tail... >
{
    using type = typename tuple_without_void< std::tuple<Types..., T>, Tail... >::type;
};

template< typename... Types >
using tuple_without_void_t = typename tuple_without_void< std::tuple<>, Types... >::type;

} // namespace meta
} // namespace SourceMap

#endif // SOURCEMAP_META_TUPLE_WITHOUT_VOID_H

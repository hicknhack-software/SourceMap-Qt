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

namespace SourceMap {
namespace meta {

/**
 * @brief meta function that retrieves the index of a type in a parameter pack
 */
template< typename, typename... >
struct type_index_of;

template< typename T, typename... Tail >
struct type_index_of< T, T, Tail...>
{
    static const std::size_t value = 0;
};

template < typename T, typename S, typename... Tail >
struct type_index_of< T, S, Tail...>
{
    static const std::size_t value = 1 + type_index_of<T, Tail...>::value;
};

/**
 * @brief meta function that retrieves the index of a type in a tuple
 */
template < typename T, typename Tuple >
struct tuple_index_of;

template < typename T, typename... Types >
struct tuple_index_of< T, std::tuple<Types...> >
{
    static const std::size_t value = type_index_of<T, Types...>::value;
};

// - Does not work in Visual Studio 2013 compiler -
//template< typename T, typename... Types >
//constexpr std::size_t type_index_of_v() { return type_index_of< T, Types... >::value; }

} // namespace meta
} // namespace SourceMap


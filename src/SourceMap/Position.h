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

namespace SourceMap {

/**
 * @brief line and column position in a text file
 *
 * the first line in a file is 1 (this is not enforced)
 * the first column on a line is 1 (0 marks an invalid position)
 *
 * this is a pure data container structure
 */
struct Position
{
    /// constructs an invalid position
    Position() {}

    /// constructs a valid position
    Position(int line, int column)
        : line(line), column(column)
    {}

    /// checks for a valid position
    inline auto isValid() const -> bool { return 0 != column; }

    int line{};
    int column{};
};

inline auto operator==(const Position& l, const Position& r) -> bool
{
    return (l.line == r.line) && (l.column == r.column);
}

inline auto operator!=(const Position& l, const Position& r) -> bool
{
    return !(l == r);
}

inline auto comparePositions(const Position &position1, const Position &position2) -> int
{
    int lineDiff = position2.line - position1.line;
    if (0 != lineDiff) return lineDiff > 0;

    int columnDiff = position2.column - position1.column;
    return columnDiff > 0;
}

} // namespace SourceMap


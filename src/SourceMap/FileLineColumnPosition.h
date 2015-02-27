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

#include <QString>

#include <vector>

namespace SourceMap {

struct FileLineColumnPosition;
using FileLineColumnPositionList = std::vector<FileLineColumnPosition>;

struct FileLineColumnPosition
{
    FileLineColumnPosition()
        : line()
        , column()
    {}

    FileLineColumnPosition(QString fileName, int line, int column)
        : fileName(fileName)
        , line(line)
        , column(column)
    {}

    QString fileName;
    int line;
    int column;
};

} // namespace SourceMap


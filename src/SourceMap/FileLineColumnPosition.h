#ifndef TWOFOLD_FILELINECOLUMNPOSITION_H
#define TWOFOLD_FILELINECOLUMNPOSITION_H

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

#endif // TWOFOLD_FILELINECOLUMNPOSITION_H

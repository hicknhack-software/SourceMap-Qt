# SourceMap-Qt
# (C) Copyright 2014 HicknHack Software GmbH
#
# The original code can be found at:
#    https://github.com/hicknhack-software/SourceMap-Qt
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This is the library project file
#
CONFIG += staticlib
TARGET = SourceMap

SOURCES += \
    SourceMap/Extension/Caller.cpp \
    SourceMap/Extension/Interpolation.cpp \
    SourceMap/intern/Base64VLQ.cpp \
    SourceMap/RevisionThree.cpp

HEADERS += \
    SourceMap/intern/Base64VLQ.h

INSTALL_HEADERS += \
    SourceMap/Extension/Base.h \
    SourceMap/Extension/Caller.h \
    SourceMap/Extension/Caller_impl.h \
    SourceMap/Extension/Interpolation.h \
    SourceMap/Extension/Interpolation_impl.h \
    SourceMap/meta/tuple_index_of.h \
    SourceMap/meta/tuple_without_void.h \
    SourceMap/Data.h \
    SourceMap/Data_impl.h \
    SourceMap/Entry.h \
    SourceMap/Extensions.h \
    SourceMap/FilePosition.h \
    SourceMap/Mapping.h \
    SourceMap/Mapping_impl.h \
    SourceMap/Position.h \
    SourceMap/RevisionThree.h \
    SourceMap/RevisionThree_impl.h

include(../build/qmake/_lib.pri)

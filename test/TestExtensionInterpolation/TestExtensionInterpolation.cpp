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
#include "SourceMap/Extension/Interpolation.h"

#include "SourceMap/Entry.h"
#include "SourceMap/Mapping.h"
#include "SourceMap/RevisionThree.h"

#include <QObject>
#include <QString>
#include <QtTest>

#include <tuple>

class TestExtensionInterpolation : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void value();
    void entry();
    void mapping();
    void revisionThree();
};

namespace {

using Interpolation = SourceMap::Interpolation;
using Mapping = SourceMap::Mapping< SourceMap::Extension::Interpolation >;
using Entry = Mapping::Entry;
using Data = Mapping::Data;
using EntryList = Mapping::EntryList;

#define SOURCE_ONE "sourceOne"
#define SOURCE_TWO "sourceTwo"
#define GENERATED "generated"
#define SYMBOL "symbol"

EntryList buildEntries()
{
    return {{
        Entry{{1,1}},
        Entry{{1,10},{SOURCE_ONE,{1,1}}, std::make_tuple(Interpolation::OneToOne)},
        Entry{{1,15}},
        Entry{{2,10},{SOURCE_TWO,{1,1}}, SYMBOL, std::make_tuple(Interpolation::OneToOne)},
        Entry{{2,1}},
        Entry{{2,20}},
    }};
}

} // namespace

void TestExtensionInterpolation::value()
{
    auto p1 = Interpolation{};
    QCOMPARE(p1, Interpolation::None);

    auto p2 = Interpolation::OneToOne;
    QCOMPARE(p2, Interpolation::OneToOne);
}

#define TEST_STR1 "hello"

void TestExtensionInterpolation::entry()
{
    using InterpolationExt = SourceMap::Extension::Interpolation;
    using SourceMap::get;

    auto p2 = Entry{{3,4}};
    QCOMPARE(get<InterpolationExt>(p2), Interpolation::None);

    auto p4 = Entry{{3,4}, {TEST_STR1, {6,4}}, TEST_STR1, std::make_tuple(Interpolation::OneToOne)};
    QCOMPARE(get<InterpolationExt>(p4), Interpolation::OneToOne);
}

void TestExtensionInterpolation::mapping()
{
    using FilePosition = SourceMap::FilePosition;

    auto map = Mapping{buildEntries()};
    auto p1 = SourceMap::getOriginalPositionFromGenerated(map, {1,5});
    QVERIFY(!p1.isValid());

    auto p2 = SourceMap::getOriginalPositionFromGenerated(map, {1,13});
    QVERIFY(p2.isValid());
    QCOMPARE(p2, FilePosition(SOURCE_ONE, {1,4}));
}

void TestExtensionInterpolation::revisionThree()
{
    using FilePosition = SourceMap::FilePosition;
    using RevisionThree = SourceMap::RevisionThree;

    auto m = Mapping{buildEntries()};

    RevisionThree r3;
    QCOMPARE(r3.version(), 3);

    r3.storeFile(GENERATED);
    QCOMPARE(r3.file(), QString(GENERATED));

    r3.encodeMappings(m);
    QCOMPARE(r3.sources().size(), 2);
    QCOMPARE(r3.names().size(), 1);

    auto bytes = r3.toJson();

    QJsonParseError error;
    auto rr = RevisionThree::fromJson(bytes, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QCOMPARE(rr.names(), r3.names());

    auto rm = Mapping{ rr.decodedMappings< Data >() };
    QCOMPARE(rm.data().entries().size(), m.data().entries().size());
    QCOMPARE(rm.originalNames(), m.originalNames());

    auto p2 = SourceMap::getOriginalPositionFromGenerated(rm, {1,13});
    QVERIFY(p2.isValid());
    QCOMPARE(p2, FilePosition(SOURCE_ONE, {1,4}));
}

QTEST_GUILESS_MAIN(TestExtensionInterpolation)

#include "TestExtensionInterpolation.moc"

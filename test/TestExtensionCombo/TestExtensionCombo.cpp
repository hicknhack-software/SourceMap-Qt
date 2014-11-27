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
#include "SourceMap/Extension/Caller.h"
#include "SourceMap/Extension/Interpolation.h"

#include "SourceMap/Entry.h"
#include "SourceMap/Mapping.h"
#include "SourceMap/RevisionThree.h"

#include <QObject>
#include <QString>
#include <QtTest>

#include <tuple>

class TestExtensionCombo : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void entry();
    void mapping();
    void revisionThree();
};

namespace {

using ExtInterpolation = SourceMap::Extension::Interpolation;
using Interpolation = SourceMap::Interpolation;

using ExtCaller = SourceMap::Extension::Caller;
using Caller = SourceMap::Caller;
using CallerList = SourceMap::CallerList;
using CallerIndex = SourceMap::CallerIndex;

using Mapping = SourceMap::Mapping< ExtInterpolation, ExtCaller >;
using Data = Mapping::Data;
using Entry = Mapping::Entry;
using EntryList = Mapping::EntryList;

#define SOURCE_ONE "sourceOne"
#define SOURCE_TWO "sourceTwo"
#define GENERATED "generated"
#define SYMBOL "symbol"

EntryList buildEntries()
{
    return {{
            Entry{{1,1}},
            Entry{{1,10},{SOURCE_ONE,{1,1}}, std::make_tuple(Interpolation::OneToOne, CallerIndex{0})},
            Entry{{1,15}},
            Entry{{2,10},{SOURCE_TWO,{1,1}}, SYMBOL, std::make_tuple(Interpolation::OneToOne, CallerIndex{2})},
            Entry{{2,1}},
            Entry{{2,20}},
        }};
}

CallerList buildCallerList()
{
    return {{
            Caller{{SOURCE_TWO,{20,1}}, CallerIndex{-1}},
            Caller{{SOURCE_TWO,{40,1}}, CallerIndex{-1}},
            Caller{{SOURCE_TWO,{40,10}}, CallerIndex{1}},
        }};
}

} // namespace

#define TEST_STR1 "hello"

void TestExtensionCombo::entry()
{
    using SourceMap::get;

    auto p2 = Entry{{3,4}};
    QCOMPARE(get<ExtCaller>(p2).value, InvalidCallerIndex);
    QCOMPARE(get<ExtInterpolation>(p2), Interpolation::None);

    auto p4 = Entry{{3,4}, {TEST_STR1, {6,4}}, TEST_STR1, std::make_tuple(Interpolation::OneToOne, CallerIndex{32})};
    QCOMPARE(get<ExtCaller>(p4).value, 32);
    QCOMPARE(get<ExtInterpolation>(p4), Interpolation::OneToOne);
}

void TestExtensionCombo::mapping()
{
    using FilePosition = SourceMap::FilePosition;

    const auto map = Mapping{{buildEntries(), std::make_tuple(buildCallerList())}};
    auto e1 = map.findEntryByGenerated({1,5});
    QVERIFY(e1 != nullptr);

    auto p1 = SourceMap::buildCallerStack(map.data(), e1);
    QVERIFY(p1.empty());

    auto e2 = map.findEntryByGenerated({2,13});
    QVERIFY(e2 != nullptr);

    auto p2 = SourceMap::buildCallerStack(map.data(), e2);
    QCOMPARE(p2.size(), 2u);
    QCOMPARE(p2[0], FilePosition(SOURCE_TWO, {40,10}));

    auto o1 = SourceMap::getOriginalPositionFromGenerated(map, {1,5});
    QVERIFY(!o1.isValid());

    auto o2 = SourceMap::getOriginalPositionFromGenerated(map, {1,13});
    QVERIFY(o2.isValid());
    QCOMPARE(o2, FilePosition(SOURCE_ONE, {1,4}));
}

void TestExtensionCombo::revisionThree()
{
    using FilePosition = SourceMap::FilePosition;
    using RevisionThree = SourceMap::RevisionThree;

    auto m = Mapping{{buildEntries(), std::make_tuple(buildCallerList())}};

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
    QCOMPARE(rm.data().entries.size(), m.data().entries.size());
    QCOMPARE(rm.originalNames(), m.originalNames());

    const auto* e2 = rm.findEntryByGenerated({2,13});
    QVERIFY(e2 != nullptr);

    auto p2 = SourceMap::buildCallerStack(rm.data(), e2);
    QCOMPARE(p2.size(), 2u);
    QCOMPARE(p2[0], FilePosition(SOURCE_TWO, {40,10}));
}

QTEST_GUILESS_MAIN(TestExtensionCombo)

#include "TestExtensionCombo.moc"



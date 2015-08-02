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
#include "SourceMap/Mapping.h"

#include <QObject>
#include <QString>
#include <QtTest>

using namespace SourceMap;

class TestMapping : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void construct();
    void findEntry();
};

namespace {

#define SOURCE_ONE "sourceOne"
#define SOURCE_TWO "sourceTwo"

EntryList<> buildEntries()
{
    using Entry = SourceMap::Entry<>;

    return {{
        Entry{{1,1}},
        Entry{{1,10},{SOURCE_ONE,{1,1}}},
        Entry{{1,15}},
        Entry{{2,10},{SOURCE_TWO,{1,1}}},
        Entry{{2,1}},
        Entry{{2,20}},
    }};
}

} // namespace

void TestMapping::construct()
{
    using Mapping = SourceMap::Mapping<>;
    auto empty = Mapping();
    Q_UNUSED(empty);

    auto entries = buildEntries();
    auto moved = Mapping{buildEntries()};
    QCOMPARE(moved.data().entries.size(), entries.size());
    QCOMPARE(moved.originalNames().size(), 2);

    auto copied = Mapping{entries};
    QCOMPARE(copied.data().entries.size(), entries.size());
    QCOMPARE(copied.originalNames().size(), 2);

    auto sorted = copied.data().entries;
    QCOMPARE(sorted[3].generated, entries[4].generated);
}

void TestMapping::findEntry()
{
    using Mapping = SourceMap::Mapping<>;

    auto map = Mapping{buildEntries()};
    auto p1 = map.findEntryByGenerated({1,9});
    QVERIFY2(p1 != nullptr, "not null");
    QCOMPARE(p1->generated, Position(1,1));

    auto p2 = map.findEntryByGenerated({1,10});
    QVERIFY2(p2 != nullptr, "not null");
    QCOMPARE(p2->generated, Position(1,10));

    auto p3 = map.findEntryByGenerated({10,100});
    QVERIFY2(p3 != nullptr, "not null");
    QCOMPARE(p3->generated, Position(2,20));
}

QTEST_GUILESS_MAIN(TestMapping)

#include "TestMapping.moc"
